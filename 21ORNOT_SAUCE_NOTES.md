# 21orNot Sauce Extraction Notes

Extracted core architecture logic for porting the ID scanner / age verification system to a TealClaw `tc-app` or `tc-flow`.

## 1. Camera & Image Parsing ("The Sauce")
- **Video Capture:** The app initializes the camera stream (`getUserMedia` requesting the environment-facing camera) to a `<video>` tag.
- **Frame Extraction:** On "Scan" action, it reads the current frame from the video tag and draws it to a hidden `<canvas>`, scaled down to a max dimension of 1800px. It then extracts the frame as a Base64-encoded JPEG image (`canvas.toDataURL('image/jpeg', 0.9)`).
- **Client-Side Barcode Check:** Before hitting the AI, it checks if the browser natively supports the `BarcodeDetector` API. If supported, it attempts to read `pdf417` or AAMVA-compliant barcodes directly from the canvas. If successful, it parses the encoded strings (like `DAC` = First Name, `DBB` = DOB, `DCS` = Last Name) to extract the data with zero latency.
- **Fallback / Vision Scan:** If the Barcode API isn't supported or fails to read anything, it hands the Base64 image over to Groq's Vision API.

## 2. OCR & Vision Logic (Date of Birth Extraction)
- **AI Model:** Uses Groq's Vision capabilities (`meta-llama/llama-4-scout-17b-16e-instruct` or equivalent via Groq API).
- **Prompt Engineering:** A highly structured prompt is sent alongside the image:
  - Injects `todayStr` so the AI knows the current date.
  - Commands the AI to extract fields (first_name, last_name, dob, state, etc.) and output strictly as a valid JSON object.
  - Specifically looks for `is_expired` and an `under_21_until` date.
  - **Hologram DOB Check:** Explicitly instructs the AI to look for *two* DOB values: the standard printed one and any secondary ghost/holographic printed date. The AI flags `"dob_mismatch": true` if they differ (a major red flag for fakes).
- **Age Calculation:** The DOB string (e.g. `MMDDYYYY`) is passed to a `calcAge` function that converts the string into a Date object and mathematically compares it to `new Date()`, checking the month/day overlap to adjust the age if their birthday hasn't occurred yet this year.

## 3. Visual & Haptic Feedback Mechanics
The UI overlays color blocks and triggers specific vibration patterns based on the calculated age:
- **Green (21+):** 
  - Visual: Applies `.screen-green` to the body, casting a green glow (`#10b981`).
  - Haptics: Two confident thumps (`navigator.vibrate([200, 80, 200])`) - "You're good."
- **Yellow (18-20):**
  - Visual: Applies `.screen-yellow`, casting a yellow/warning glow (`#eab308`).
  - Haptics: Three quick nudges (`navigator.vibrate([120, 60, 120, 60, 120])`) - "Hold on, check the age."
- **Red (Under 18):**
  - Visual: Applies `.screen-red`, casting a red glow (`#ef4444`).
  - Haptics: Rapid staccato buzz (`navigator.vibrate([80, 40, 80, 40, 80, 40, 80])`) - "Stop, minor."
- **Orange (Expired / Fake / Suspicious):**
  - If the ID is expired, or if the dual-DOB check fails (`LIKELY_FAKE`), an orange overlay is used alongside an urgent vibration (`[200, 100, 200, 100, 200, 100, 200]`).
