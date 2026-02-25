# ChatFormed Architecture & "Sauce" Notes
*Extracted Logic for TealClaw JavaScript Porting*

This document breaks down the core logic and mechanics of the ChatFormed codebase. It covers how the application defines, extracts, and validates dynamic fields, flags PII/sensitive data, and detects conversational completion to trigger external workflows like Webhooks/AgentMail.

---

## 1. Dynamic Field Definition, Extraction, and Validation

### A. Field Definition (`builder_ai.py`)
ChatFormed uses an agentic wizard to dynamically build forms:
- **Context Scraping:** The system starts by asking for a website URL. If provided, it scrapes the domain to extract logo, brand colors, business description, and FAQs to contextually ground the subsequent form chatbot (`__SCRAPE_WEBSITE_DEEP__`).
- **Step-by-Step Collection:** The Builder AI walks the user through "Field Collection" and "Field Details" steps. For each identified field, the AI asks specific formatting questions (e.g., "MM/DD/YYYY vs YYYY-MM-DD", "First Last vs Last, First") and prompts the user for custom "Do's and Don'ts" (e.g., "Require hyphens", "No PO Boxes").
- **Universal Processing:** If a document is uploaded instead of built via chat, `universal_form_processor.py` acts as a router to extract existing fillable fields from formats like PDF, DOCX, HTML, and ODT.

### B. Extraction Logic (`chat_ai.py` - `extract_data_from_message`)
Field extraction relies on a combination of exact pattern matching and positional assignment:
1. **N/A Handling:** Scans for exact phrases (`"N/A"`, `"skip"`) or contextual phrases (`"doesn't apply to me"`). If matched, it marks the next unfilled field as "N/A".
2. **Regex Type Detection:** Runs regex to detect Emails (`[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+`) or Phone Numbers. If detected, it maps the extracted value directly to the next required email or phone field.
3. **Sequential Assignment:** If no strict type matches, the system evaluates the next expected unfilled field. It evaluates conditional logic (`evaluate_conditions`)—like "Only show Field B if Field A is 'Yes'"—to determine what the actual "next" field is.

### C. Validation Mechanism
Validation operates on two tiers:
- **Standard LLM Validation:** Simple checks (e.g., checking if an email has an `@`, or verifying date structures via `parse_flexible_date`). 
- **Secure Server-Side Validation (Bypassing LLM):** If a field is marked as secure (e.g., SSN), the user's input is **not** evaluated by the LLM. Instead, it is routed to `validate_secure_field()`, which validates format (e.g., `^(\d{3}[-]?\d{2}[-]?\d{4})$` for SSNs) entirely on the backend to prevent sensitive data leakage into the AI context window.

---

## 2. PII & Sensitive Data Detection

The platform uses rigorous checks in `content_screening.py` to prevent processing of illegal, dangerous, or highly regulated data.

### A. Document/Context Screening (Weighted Keywords)
When analyzing form text/setup, the system calculates a weighted score based on dictionary matches (High=3.0, Medium=1.5, Low=0.5), normalized by document length.
- **Medical Forms (0.80 Threshold):** Flags HIPAA-related terms (`"protected health information"`, `"diagnosis"`, `"cpt code"`). If the confidence threshold is met, the form is completely rejected, citing that ChatFormed is not HIPAA-compliant.
- **Immigration Forms (0.75 Threshold):** Rejects high-stakes forms (`"visa petition"`, `"i-485"`, `"asylum"`). The logic explicitly notes that immigration forms are blocked because mistakes carry extreme legal consequences.

### B. Banned Field Collection 
During user interaction, the system actively scrubs restricted inputs via the `BANNED_FIELDS` registry:
- **Credit Card Numbers:** Matched via 16-digit regex patterns, Amex patterns, or keywords like `"cvv"`. If detected, the field is skipped and replaced with a warning: *"Credit card information cannot be collected through chat... integrate Stripe checkout instead."* (The backend does have a Luhn checksum validator in `chat_ai.py` just in case).
- **Bank Accounts:** Matched via routing/account number regex (`\b\d{8,17}\b`) and SWIFT/IBAN keywords.
- **Passwords:** Keywords like `"password"`, `"pin number"`, or `"login credentials"` are strictly prohibited from collection.

*(Note: Social Security Numbers are permitted by the screening rules since they are required for tax/employment forms, but they utilize the secure validation bypass mentioned above).*

---

## 3. Conversational Completion & Webhook Triggers

The system does not rely on a single rigid trigger to know when a form is complete. It uses a dual-condition check evaluated continuously in `chat_ai.py`:

### A. Completion Conditions (`is_complete`)
1. **Mathematical Progress:** `progress >= 1.0` 
   - Progress is calculated as `count_filled / count_visible_fields`. 
   - `count_visible_fields` evaluates dynamic conditions in real-time, meaning the required field count can grow or shrink depending on user answers.
2. **Heuristic AI Override:** `ai_says_complete`
   - The system intercepts the AI's generated response and checks it against a list of hardcoded completion phrases (e.g., `"all fields are now complete"`, `"ready to download"`, `"here's a summary"`).
   - If the AI naturally concludes the conversation with one of these phrases, it overrides the progress metric and sets completion to 100%.

### B. Triggering Webhooks / Event Notifications
Once `is_complete == True`, the backend (`app.py` / `webhook_sender.py`) processes the final payload:
- **Webhook Dispatch:** A payload containing the form details, filled data, and generated PDF URL is dispatched to the configured business endpoints.
- **Security:** The payload is signed with an HMAC SHA256 signature (`X-ChatFormed-Signature`).
- **Resilience:** Delivery utilizes exponential backoff, retrying up to 3 times (at 1-minute, 5-minute, and 15-minute intervals) if the endpoint times out or returns a 5xx error. Client-side errors (4xx) do not trigger retries.