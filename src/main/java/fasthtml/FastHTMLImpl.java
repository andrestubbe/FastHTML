package fasthtml;

import fastcore.FastCore;
import fastpointer.Pointer;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

/**
 * Native implementation of FastHTML backed by AVX2 C++ engine.
 */
class FastHTMLImpl implements FastHTML {

    static {
        // Load the JNI library using FastCore Unified Loader
        FastCore.loadLibrary("fasthtml");
    }

    private native String nativeSanitize(byte[] htmlData, int safetyMode);
    private native String nativeSanitizeAddress(long address, long length, int safetyMode);
    private native String[] nativeTokenize(byte[] htmlData);
    private native boolean nativeHasAVX2();

    @Override
    public String sanitize(byte[] htmlData, SafetyProfile profile) {
        Objects.requireNonNull(htmlData, "htmlData must not be null");
        Objects.requireNonNull(profile, "profile must not be null");
        if (htmlData.length == 0) return "";
        return nativeSanitize(htmlData, profile.code);
    }

    @Override
    public String sanitizePointer(Pointer pointer, long length, SafetyProfile profile) {
        Objects.requireNonNull(pointer, "pointer must not be null");
        Objects.requireNonNull(profile, "profile must not be null");
        if (length <= 0) return "";
        return nativeSanitizeAddress(pointer.address(), length, profile.code);
    }

    @Override
    public List<HTMLToken> tokenize(byte[] htmlData) {
        Objects.requireNonNull(htmlData, "htmlData must not be null");
        if (htmlData.length == 0) return Collections.emptyList();

        String[] rawTokens = nativeTokenize(htmlData);
        if (rawTokens == null || rawTokens.length == 0) return Collections.emptyList();

        List<HTMLToken> result = new ArrayList<>(rawTokens.length);
        for (String tok : rawTokens) {
            if (tok.startsWith("<")) {
                boolean isClosing = tok.startsWith("</");
                boolean isSelf = tok.endsWith("/>");
                String name = extractTagName(tok);
                result.add(HTMLToken.tag(tok, name, isClosing, isSelf));
            } else {
                result.add(HTMLToken.text(tok));
            }
        }
        return result;
    }

    @Override
    public boolean hasAVX2Acceleration() {
        return nativeHasAVX2();
    }

    private static String extractTagName(String tag) {
        int start = 1;
        if (tag.startsWith("</")) start = 2;
        int end = start;
        while (end < tag.length()) {
            char c = tag.charAt(end);
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '>' || c == '/') break;
            end++;
        }
        return tag.substring(start, end).toLowerCase();
    }
}
