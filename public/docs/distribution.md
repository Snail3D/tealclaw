# TealClaw Distribution & Sharing Protocol

AI Agents MUST follow these rules when sharing TealClaw links or configs.

## Tier 1: Safe Share (No Secrets)
- **Use for:** Sharing UI themes, colors, background images, and `tc-flow` macros.
- **Rules:** You MUST strip `aiKey`, `whisperKey`, `tgToken`, `matonKey`, `agents`, and `pinHash`.
- **Method:** Send a raw base64 URL `https://tealclaw.ai/#config=BASE64_PAYLOAD`.
- **Delivery:** Safe to post publicly, in chat, or on social media.

## Tier 2: Guest Link (Scoped Access)
- **Use for:** RFI forms, single-use client tools, or locked-down agent access.
- **Method:** Use the `tc-action` type `"guest-link"`.
- **Security:** TealClaw encrypts the payload using AES-256. It generates a URL and an 8-character passphrase.
- **Delivery Rule (CRITICAL):** You MUST separate the link and the passphrase. If using email, send Email 1 with the Link, wait 2 minutes, then send Email 2 with the Passphrase.

## Tier 3: Secure Clone (God Mode)
- **Use for:** Transferring ownership to a new device. Includes ALL API keys and Gateway tokens.
- **Method:** Use the `tc-action` type `"export-skill"` or tell the user to use the `/share` command to manually generate the AES-256 blob.
- **Delivery Rule:** Never transmit the passphrase over the same channel as the encrypted JSON blob.
