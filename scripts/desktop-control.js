(function(global){
  'use strict';

  function nowIso(){
    return new Date().toISOString();
  }

  function normalizeExecResult(res){
    if(!res||typeof res!=='object')return {ok:false,stdout:'',stderr:'',code:1,error:'No execution result'};
    return {
      ok:res.ok===true,
      stdout:String(res.stdout||''),
      stderr:String(res.stderr||''),
      code:Number.isFinite(res.code)?res.code:(res.ok===true?0:1),
      error:res.error?String(res.error):''
    };
  }

  function permissionEntry(state,detail){
    return {state,detail:detail||''};
  }

  function deniedByMessage(msg){
    const t=String(msg||'').toLowerCase();
    return /not authorized|not allowed|assistive access|not permitted|operation not permitted|\(-1743\)|\(-1719\)/.test(t);
  }

  class DesktopControlBackend {
    constructor(id,label){
      this.id=id;
      this.label=label;
    }

    async status(){
      return {
        backend:this.id,
        backendLabel:this.label,
        available:false,
        checkedAt:nowIso(),
        permissions:{
          accessibility:permissionEntry('unknown','Not checked'),
          screenRecording:permissionEntry('unknown','Not checked'),
          automation:permissionEntry('unknown','Not checked')
        },
        notes:['Desktop backend does not implement status() yet.']
      };
    }

    async start(ctx){
      const status=await this.status(ctx);
      return {ok:false,status,message:'Desktop backend start() not implemented.'};
    }

    async stop(ctx){
      const status=await this.status(ctx);
      return {ok:true,status,message:'Desktop control stopped.'};
    }
  }

  class NoopDesktopBackend extends DesktopControlBackend {
    constructor(){
      super('noop','Unavailable');
    }

    async status(ctx){
      const isMac=ctx?.platform==='macos';
      const detail=isMac
        ?'No local desktop bridge is connected. Desktop control remains disabled in browser-only mode.'
        :'Desktop control foundation currently targets macOS first.';
      return {
        backend:this.id,
        backendLabel:this.label,
        available:false,
        checkedAt:nowIso(),
        permissions:{
          accessibility:permissionEntry('unknown',detail),
          screenRecording:permissionEntry('unknown','Requires native runtime checks.'),
          automation:permissionEntry('unknown',detail)
        },
        notes:[
          'Local desktop bridge missing.',
          'No privilege escalation attempted.'
        ]
      };
    }
  }

  class MacOsSystemEventsBackend extends DesktopControlBackend {
    constructor(){
      super('macos-system-events','macOS System Events');
    }

    async runAppleScript(script,ctx){
      const execAppleScript=ctx?.execAppleScript;
      if(typeof execAppleScript!=='function'){
        return {ok:false,stdout:'',stderr:'',code:1,error:'No AppleScript bridge is available'};
      }
      try{
        return normalizeExecResult(await execAppleScript(script,{timeoutMs:4000}));
      }catch(err){
        return {ok:false,stdout:'',stderr:'',code:1,error:String(err?.message||err||'AppleScript execution failed')};
      }
    }

    async status(ctx){
      const isMac=ctx?.platform==='macos';
      if(!isMac){
        return {
          backend:this.id,
          backendLabel:this.label,
          available:false,
          checkedAt:nowIso(),
          permissions:{
            accessibility:permissionEntry('unsupported','Desktop control currently supports macOS only.'),
            screenRecording:permissionEntry('unsupported','Desktop control currently supports macOS only.'),
            automation:permissionEntry('unsupported','Desktop control currently supports macOS only.')
          },
          notes:['Platform is not macOS.']
        };
      }

      const base={
        backend:this.id,
        backendLabel:this.label,
        available:false,
        checkedAt:nowIso(),
        permissions:{
          accessibility:permissionEntry('unknown','Not checked yet.'),
          screenRecording:permissionEntry('unknown','Manual grant required in System Settings for full-screen visibility.'),
          automation:permissionEntry('unknown','Not checked yet.')
        },
        notes:[]
      };

      const ping=await this.runAppleScript('return "ok"',ctx);
      if(!ping.ok){
        base.notes.push('AppleScript bridge unavailable: '+(ping.error||ping.stderr||'execution failed'));
        return base;
      }

      base.available=true;

      const accessibility=await this.runAppleScript('tell application "System Events" to return UI elements enabled',ctx);
      if(accessibility.ok){
        const value=accessibility.stdout.trim().toLowerCase();
        if(value==='true')base.permissions.accessibility=permissionEntry('granted','System Events UI scripting is enabled.');
        else if(value==='false')base.permissions.accessibility=permissionEntry('denied','Enable Accessibility for TealClaw host app in Privacy & Security.');
        else base.permissions.accessibility=permissionEntry('unknown','Unexpected Accessibility response: '+(accessibility.stdout.trim()||'empty'));
      }else if(deniedByMessage(accessibility.error||accessibility.stderr)){
        base.permissions.accessibility=permissionEntry('denied','Accessibility permission was denied for System Events checks.');
      }else{
        base.permissions.accessibility=permissionEntry('unknown',accessibility.error||accessibility.stderr||'Accessibility check failed.');
      }

      const automation=await this.runAppleScript('tell application "System Events" to count every process',ctx);
      if(automation.ok){
        base.permissions.automation=permissionEntry('granted','Automation to System Events appears available.');
      }else if(deniedByMessage(automation.error||automation.stderr)){
        base.permissions.automation=permissionEntry('denied','Allow Automation access to System Events for TealClaw host app.');
      }else{
        base.permissions.automation=permissionEntry('unknown',automation.error||automation.stderr||'Automation check failed.');
      }

      base.notes.push('Screen Recording state is reported as manual/unknown until native APIs are wired.');
      base.notes.push('No privilege escalation performed. TealClaw never executes AppleScript with administrator privileges.');
      return base;
    }

    async start(ctx){
      const status=await this.status(ctx);
      if(!status.available){
        return {ok:false,status,message:'Desktop control bridge unavailable on this runtime.'};
      }
      return {ok:true,status,message:'Desktop control enabled (foundation mode).'};
    }

    async stop(ctx){
      const status=await this.status(ctx);
      return {ok:true,status,message:'Desktop control disabled.'};
    }
  }

  class DesktopControlAdapter {
    constructor(opts){
      const options=opts||{};
      this.backends=new Map();
      this.preferredBackend=options.preferredBackend||'';
      this.execAppleScript=typeof options.execAppleScript==='function'?options.execAppleScript:null;
      this.platform=options.platform||((typeof navigator!=='undefined'&&/mac/i.test(navigator.platform||''))?'macos':'other');
      this.enabled=false;
      this.lastStatus=null;
    }

    registerBackend(backend){
      if(!backend||!backend.id)return;
      this.backends.set(backend.id,backend);
    }

    getContext(extra){
      const out={
        platform:this.platform,
        execAppleScript:this.execAppleScript
      };
      if(extra&&typeof extra==='object')Object.assign(out,extra);
      return out;
    }

    resolveBackend(){
      if(this.preferredBackend&&this.backends.has(this.preferredBackend))return this.backends.get(this.preferredBackend);
      if(this.platform==='macos'&&this.backends.has('macos-system-events'))return this.backends.get('macos-system-events');
      return this.backends.get('noop')||null;
    }

    async status(extra){
      const backend=this.resolveBackend();
      if(!backend){
        this.lastStatus={
          backend:'none',
          backendLabel:'Unavailable',
          available:false,
          checkedAt:nowIso(),
          permissions:{
            accessibility:permissionEntry('unknown','No backend registered.'),
            screenRecording:permissionEntry('unknown','No backend registered.'),
            automation:permissionEntry('unknown','No backend registered.')
          },
          notes:['Desktop adapter has no registered backend.']
        };
        return this.lastStatus;
      }
      this.lastStatus=await backend.status(this.getContext(extra));
      return this.lastStatus;
    }

    async on(extra){
      const backend=this.resolveBackend();
      if(!backend)return {ok:false,message:'No desktop backend available.',status:await this.status(extra)};
      const out=await backend.start(this.getContext(extra));
      this.enabled=!!out.ok;
      this.lastStatus=out.status||this.lastStatus;
      return out;
    }

    async off(extra){
      const backend=this.resolveBackend();
      if(!backend)return {ok:true,message:'Desktop control already off.',status:await this.status(extra)};
      const out=await backend.stop(this.getContext(extra));
      this.enabled=false;
      this.lastStatus=out.status||this.lastStatus;
      return out;
    }
  }

  function createDesktopControlAdapter(opts){
    const adapter=new DesktopControlAdapter(opts||{});
    adapter.registerBackend(new NoopDesktopBackend());
    adapter.registerBackend(new MacOsSystemEventsBackend());
    return adapter;
  }

  global.TealClawDesktopControl={
    createDesktopControlAdapter,
    DesktopControlAdapter,
    DesktopControlBackend,
    MacOsSystemEventsBackend,
    NoopDesktopBackend
  };
})(window);
