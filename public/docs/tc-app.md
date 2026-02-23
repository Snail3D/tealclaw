# tc-app Engine (Sandboxed Mini-Apps)

You can write fully interactive HTML/JS/CSS applications that render directly inside a TealClaw chat bubble.

## Architecture
Return a fenced markdown block tagged with ```tc-app containing standard HTML.

## Security & Mechanics
- The code runs inside a sandboxed `<iframe>` with `allow-scripts`.
- It cannot access TealClaw's parent DOM or API keys.
- It automatically scales to fit the chat bubble (Flexbox/Grid recommended).

## The postMessage Bridge (CRITICAL)
To allow the app to talk back to you (the AI agent), the iframe can send a `postMessage` to the parent window.

**Javascript Example inside the tc-app:**
```javascript
function sendDataToAgent(result) {
  window.parent.postMessage({ 
    type: 'tc-app-send', 
    text: `The user clicked the button. Result: ${result}` 
  }, '*');
}
```
When this fires, TealClaw intercepts it and automatically sends the text to the AI over the WebSocket as if the user typed it.

Use this to build calculators, interactive forms, and mini-games that interact with the agent conversation.
