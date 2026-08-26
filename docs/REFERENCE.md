# FastHTML API Reference

## Class: `fasthtml.FastHTML`

### Factory Methods
- `public static FastHTML getInstance()` / `public static FastHTML open()`: Returns thread-safe singleton instance.

### Methods
- `public boolean hasAVX2()`: Returns `true` if native AVX2 256-bit vector extensions are available and active.
- `public String sanitize(String html, SafetyProfile profile)`: Sanitizes HTML string using specified safety profile.
- `public String sanitize(long address, long length, SafetyProfile profile)`: Zero-copy sanitization directly on off-heap memory buffers.
- `public List<HTMLToken> tokenize(String html)`: Parses HTML stream into high-speed structured tokens.

---

## Enum: `fasthtml.FastHTML.SafetyProfile`

- `STRICT_WHITELIST`: Allows standard semantic tags (`p`, `div`, `a`, `span`, `img`, `h1`-`h6`, `table`, etc.) and scrubs unlisted elements.
- `RELAXED`: Preserves layout markup while removing dangerous blocks (`<script>`, `<style>`, `<iframe>`, `<object>`, `<embed>`).
- `TEXT_ONLY`: Strips all markup, returning pure plain text.

---

## Record: `fasthtml.HTMLToken`

- `TokenType type()`: `START_TAG`, `END_TAG`, `TEXT`, `COMMENT`, `DOCTYPE`.
- `String content()`: Raw token string representation.
