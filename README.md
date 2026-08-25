# FastHTML ⚡

> **Native AVX2 Real-Time HTML Document Sanitizer, Lexer & Tokenizer Pipeline for Java 26+**

FastHTML is a zero-copy, AVX2 vector-accelerated native HTML processing engine designed for ultra-high throughput applications. It delivers sub-microsecond sanitization and tokenization to safeguard against XSS attacks, malicious script injections, and unwanted DOM nodes.

---

## Features

- ⚡ **AVX2 Vector Acceleration (256-bit SIMD)**: Rapidly scans delimiters (`<`, `>`, quotes, whitespace) with native C++ SIMD routines.
- 🛡️ **Multi-Tier Safety Profiles**:
  - `STRICT_WHITELIST`: Allows standard semantic tags (`p`, `div`, `a`, `span`, `img`, `h1`-`h6`, `table`, etc.) and strips unlisted elements.
  - `RELAXED`: Retains safe structures while scrubbing dangerous execution tags (`<script>`, `<style>`, `<iframe>`, `<object>`, `<embed>`).
  - `TEXT_ONLY`: Strips all HTML markup, preserving pure textual content.
- 🚫 **Active Vector Sanitizer**: Automatically strips inline event handlers (`onclick`, `onload`, `onerror`) and sanitizes unsafe URI schemes (`javascript:`).
- 🔗 **Zero-Copy Memory Addressing**: Native interop with direct native memory pointers (`FastPointer`, `FastMemory`) for zero-allocation stream processing.
- 🎨 **FastJava Ecosystem Native**: Seamlessly interoperates with `FastCore`, `FastPointer`, `FastMemory`, `FastSIMD`, and `FastANSI`.

---

## Quick Start

### Maven Dependency

```xml
<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>FastHTML</artifactId>
    <version>0.1.0</version>
</dependency>
```

### Usage

```java
import fasthtml.FastHTML;

public class App {
    public static void main(String[] args) {
        FastHTML fastHtml = FastHTML.getInstance();

        String dirty = "<div class=\"hero\">"
                     + "  <h1>Hello World <script>evil()</script></h1>"
                     + "  <p onclick=\"steal()\">Read <a href=\"javascript:attack()\">more</a>.</p>"
                     + "</div>";

        // Strict whitelist sanitization
        String safe = fastHtml.sanitize(dirty, FastHTML.SafetyProfile.STRICT_WHITELIST);
        System.out.println(safe);
        // Output: <div class="hero"><h1>Hello World </h1><p>Read <a href="#">more</a>.</p></div>

        // Fast zero-allocation tokenization
        var tokens = fastHtml.tokenize(safe);
        for (var token : tokens) {
            System.out.println(token.type() + ": " + token.content());
        }
    }
}
```

---

## Interactive Demo

Run the built-in terminal demo:

```bash
run-demo.bat
```

---

## License

Apache License 2.0
