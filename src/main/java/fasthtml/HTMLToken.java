package fasthtml;

/**
 * FastHTML token representation for streaming and zero-allocation document parsing.
 */
public record HTMLToken(TokenType type, String rawValue, String tagName, boolean isClosing) {

    public enum TokenType {
        START_TAG,
        END_TAG,
        SELF_CLOSING_TAG,
        TEXT,
        COMMENT,
        STRIPPED_TAG
    }

    public static HTMLToken text(String text) {
        return new HTMLToken(TokenType.TEXT, text, null, false);
    }

    public static HTMLToken tag(String raw, String name, boolean isClosing, boolean isSelfClosing) {
        TokenType type = isClosing ? TokenType.END_TAG : (isSelfClosing ? TokenType.SELF_CLOSING_TAG : TokenType.START_TAG);
        return new HTMLToken(type, raw, name, isClosing);
    }

    public static HTMLToken stripped(String raw, String name) {
        return new HTMLToken(TokenType.STRIPPED_TAG, raw, name, false);
    }
}
