const fs = require('fs');
let code = fs.readFileSync('/Users/ericwoodard/Desktop/programs/tealclaw/index.html', 'utf8');

const target1 = `      if (isOllamaGenerate) {
        const prompt = messages.map(m=>\`\${m.role}: \${typeof m.content==='string'?m.content:JSON.stringify(m.content)}\`).join('\\n\\n');
        body={model, prompt, stream: !!stream};`;

const replace1 = `      if (isOllamaGenerate) {
        let allImages=[];
        const prompt = messages.map(m=>{
          if(typeof m.content==='string')return \`\${m.role}: \${m.content}\`;
          const txt=m.content.find(p=>p.type==='text')?.text||'';
          const img=m.content.find(p=>p.type==='image_url')?.image_url?.url;
          if(img)allImages.push(img.split(',')[1]);
          return \`\${m.role}: \${txt}\`;
        }).join('\\n\\n');
        body={model, prompt, stream: !!stream};
        if(allImages.length)body.images=allImages;`;

const target2 = `      } else if (isOllamaChat) {
        body={model, messages, stream: !!stream};`;

const replace2 = `      } else if (isOllamaChat) {
        const mappedMsgs=messages.map(m=>{
          if(typeof m.content==='string')return m;
          const txt=m.content.find(p=>p.type==='text')?.text||'';
          const img=m.content.find(p=>p.type==='image_url')?.image_url?.url;
          const res={role:m.role,content:txt};
          if(img)res.images=[img.split(',')[1]];
          return res;
        });
        body={model, messages: mappedMsgs, stream: !!stream};`;

if(code.includes(target1) && code.includes(target2)){
  code = code.replace(target1, replace1);
  code = code.replace(target2, replace2);
  fs.writeFileSync('/Users/ericwoodard/Desktop/programs/tealclaw/index.html', code);
  console.log("Patch successful");
} else {
  console.log("Targets not found");
}
