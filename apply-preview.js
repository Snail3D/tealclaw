const fs = require('fs');
let html = fs.readFileSync('index.html', 'utf8');

const target = `    function renderMd(text){`;
const replace = `    function renderMd(text){
      // tc-link-preview proxy injection
      const rawUrlMatch = text.match(/^https?:\\/\\/[^\\s]+$/i);
      if (rawUrlMatch && text.trim() === rawUrlMatch[0]) {
        const pUrl = rawUrlMatch[0];
        const previewId = 'lp-' + Math.random().toString(36).substr(2,9);
        setTimeout(() => {
          fetch('https://tc-link-preview.ericwoodard84.workers.dev/?url=' + encodeURIComponent(pUrl))
            .then(r => r.json())
            .then(d => {
              const p = document.getElementById(previewId);
              if (p && !d.error) {
                const imgHtml = d.image ? '<img src="' + d.image + '" style="width:100%;max-height:160px;object-fit:cover;border-radius:var(--r-md) var(--r-md) 0 0;margin-bottom:8px">' : '';
                p.innerHTML = '<a href="' + pUrl + '" target="_blank" style="display:block;margin-top:12px;border:1px solid var(--border);border-radius:var(--r-md);text-decoration:none;color:inherit;overflow:hidden;background:var(--bg-input);transition:border 0.2s">' + imgHtml + '<div style="padding:10px"><div style="font-weight:600;font-size:14px;margin-bottom:4px;color:var(--text);white-space:nowrap;overflow:hidden;text-overflow:ellipsis">' + (d.title || new URL(pUrl).hostname) + '</div><div style="font-size:12px;color:var(--text-3);display:-webkit-box;-webkit-line-clamp:2;-webkit-box-orient:vertical;overflow:hidden">' + (d.description || pUrl) + '</div></div></a>';
                p.style.opacity = '1';
              } else if (p) p.remove();
            }).catch(() => { const p = document.getElementById(previewId); if(p) p.remove(); });
        }, 100);
        return esc(text) + '<div id="' + previewId + '" style="margin-top:12px;opacity:0.6;font-size:12px">Loading preview...</div>';
      }
`;

if (html.includes(target) && !html.includes('tc-link-preview')) {
  html = html.replace(target, replace);
  fs.writeFileSync('index.html', html);
  console.log('index.html patched');
} else {
  console.log('Target not found or already patched.');
}

let sw = fs.readFileSync('sw.js', 'utf8');
sw = sw.replace(/const CACHE='tealclaw-v(\d+)'/, (m, num) => {
  return "const CACHE='tealclaw-v" + (parseInt(num) + 1) + "'";
});
fs.writeFileSync('sw.js', sw);
console.log('sw.js bumped');
