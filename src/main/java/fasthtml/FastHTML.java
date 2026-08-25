package fasthtml;

import fastpointer.Pointer;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.stream.Stream;

/**
 * FastHTML — High-performance Native AVX2 HTML Document Sanitizer, Lexer & Tokenizer.
 */
public interface FastHTML {

    enum SafetyProfile {
        STRICT_WHITELIST(0),  // Only safe tags (a, p, h1-h6, table, strong, em, etc.) + safe attrs
        LENIENT_BLACKLIST(1), // Strips <script>, <style>, <iframe>, onclick, javascript:
        TEXT_ONLY(2);         // Strips all HTML markup completely

        public final int code;
        SafetyProfile(int code) { this.code = code; }
    }

    /**
     * Opens a new FastHTML engine instance.
     */
    static FastHTML open() {
        return new FastHTMLImpl();
    }

    /**
     * Sanitizes raw HTML bytes using the specified safety profile.
     */
    String sanitize(byte[] htmlData, SafetyProfile profile);

    /**
     * Sanitizes raw HTML bytes with STRICT_WHITELIST.
     */
    default String sanitize(byte[] htmlData) {
        return sanitize(htmlData, SafetyProfile.STRICT_WHITELIST);
    }

    /**
     * Sanitizes an HTML string with a safety profile.
     */
    default String sanitize(String html, SafetyProfile profile) {
        if (html == null || html.isEmpty()) return "";
        return sanitize(html.getBytes(StandardCharsets.UTF_8), profile);
    }

    /**
     * Sanitizes an HTML string with STRICT_WHITELIST.
     */
    default String sanitize(String html) {
        return sanitize(html, SafetyProfile.STRICT_WHITELIST);
    }

    /**
     * Sanitizes directly from off-heap memory via FastPointer.
     */
    String sanitizePointer(Pointer pointer, long length, SafetyProfile profile);

    /**
     * Tokenizes raw HTML bytes into structured tokens.
     */
    List<HTMLToken> tokenize(byte[] htmlData);

    /**
     * Streaming Tokenizer for high-throughput LLM / RAG parsing.
     */
    default Stream<HTMLToken> streamTokens(byte[] htmlData) {
        return tokenize(htmlData).stream();
    }

    /**
     * Returns true if hardware AVX2 vector SIMD acceleration is active.
     */
    boolean hasAVX2Acceleration();
}
