# tc-flow Engine (Visual Macros)

A `tc-flow` is a visual, drag-and-drop prompt chain that executes locally in the TealClaw browser.

## Architecture
Return a fenced markdown block tagged with ```tc-flow containing valid JSON.

### Nodes:
1. `input`: Prompts the user for a text string (e.g. "Client Name").
2. `sanitize`: Automatically scrubs `[EMAIL]` and `[PHONE]` from the payload using client-side regex *before* it hits an LLM.
3. `llm`: Sends the payload to the active AI provider (e.g., Groq). Uses the `system` field as the hidden prompt.
4. `action`: Triggers a native `tc-action` (like `/save` to Obsidian) using `{payload}` or `{node_X_output}` variables.
5. `output`: Drops the final payload directly into the chat timeline as a System Bubble.

## Example Payload: Secure Data Scrubber
```json
{
  "name": "Secure Scrubber",
  "nodes": [
    { "id": "1", "type": "input", "label": "Paste Customer Data" },
    { "id": "2", "type": "sanitize", "label": "Scrub PII" },
    { "id": "3", "type": "output", "label": "Print Safe Data" }
  ],
  "edges": [
    { "from": "1", "to": "2" },
    { "from": "2", "to": "3" }
  ]
}
```
