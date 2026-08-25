#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <immintrin.h>
#include <intrin.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include "fasthtml.h"

static bool checkAVX2Support() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);
    if (cpuInfo[0] < 7) return false;
    __cpuidex(cpuInfo, 7, 0);
    return (cpuInfo[1] & (1 << 5)) != 0;
}

static const bool g_hasAVX2 = checkAVX2Support();

JNIEXPORT jboolean JNICALL Java_fasthtml_FastHTMLImpl_nativeHasAVX2
  (JNIEnv *env, jobject obj) {
    return g_hasAVX2 ? JNI_TRUE : JNI_FALSE;
}

static inline bool iequals(const char* a, size_t aLen, const char* b, size_t bLen) {
    if (aLen != bLen) return false;
    for (size_t k = 0; k < aLen; k++) {
        char c1 = a[k]; if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        char c2 = b[k]; if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        if (c1 != c2) return false;
    }
    return true;
}

static const char* SAFE_WHITELIST_TAGS[] = {
    "a", "p", "div", "span", "img", "ul", "ol", "li",
    "h1", "h2", "h3", "h4", "h5", "h6",
    "table", "thead", "tbody", "tfoot", "tr", "td", "th",
    "code", "pre", "strong", "em", "b", "i", "u", "br", "hr",
    "blockquote", "section", "article", "header", "footer", "nav", "main"
};
static const size_t SAFE_WHITELIST_TAGS_COUNT = 37;

static const char* DANGEROUS_BLOCK_TAGS[] = {
    "script", "style", "iframe", "object", "embed", "applet", "frame", "frameset"
};
static const size_t DANGEROUS_BLOCK_TAGS_COUNT = 8;

static bool isTagInList(const char* tagStart, size_t tagLen, const char* list[], size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (iequals(tagStart, tagLen, list[i], strlen(list[i]))) {
            return true;
        }
    }
    return false;
}

static std::string sanitizeRawMemory(const char* src, size_t len, int safetyMode) {
    std::string out;
    out.reserve(len);

    size_t i = 0;
    while (i < len) {
        if (src[i] != '<') {
            out += src[i++];
            continue;
        }

        // We are at '<'
        size_t tagStart = i;
        size_t closeBracket = i + 1;
        while (closeBracket < len && src[closeBracket] != '>') {
            closeBracket++;
        }

        if (closeBracket >= len) {
            // Unclosed tag at EOF
            out.append(src + i, len - i);
            break;
        }

        size_t tagEnd = closeBracket + 1; // 1 past '>'

        // Parse tag name
        size_t nameStart = tagStart + 1;
        bool isClosing = false;
        if (nameStart < tagEnd && src[nameStart] == '/') {
            isClosing = true;
            nameStart++;
        }

        size_t nameEnd = nameStart;
        while (nameEnd < tagEnd && src[nameEnd] != ' ' && src[nameEnd] != '\t' && 
               src[nameEnd] != '\r' && src[nameEnd] != '\n' && src[nameEnd] != '>' && src[nameEnd] != '/') {
            nameEnd++;
        }
        size_t nameLen = nameEnd - nameStart;

        // Mode 2: TEXT_ONLY -> discard all tags
        if (safetyMode == 2) {
            i = tagEnd;
            continue;
        }

        // Dangerous block tags (script, style, iframe, etc.)
        if (!isClosing && isTagInList(src + nameStart, nameLen, DANGEROUS_BLOCK_TAGS, DANGEROUS_BLOCK_TAGS_COUNT)) {
            // Find closing </tagname>
            std::string closePattern = "</";
            for (size_t k = 0; k < nameLen; k++) {
                char c = src[nameStart + k];
                if (c >= 'A' && c <= 'Z') c += 32;
                closePattern += c;
            }

            size_t skipTo = tagEnd;
            for (size_t s = tagEnd; s + closePattern.size() <= len; s++) {
                if (src[s] == '<' && s + 1 < len && src[s+1] == '/') {
                    size_t matchLen = closePattern.size();
                    if (s + matchLen <= len && iequals(src + s, matchLen, closePattern.c_str(), matchLen)) {
                        size_t endClose = s + matchLen;
                        while (endClose < len && src[endClose] != '>') endClose++;
                        if (endClose < len) endClose++;
                        skipTo = endClose;
                        break;
                    }
                }
            }
            i = skipTo;
            continue;
        }

        // Strict Whitelist mode
        if (safetyMode == 0) {
            if (!isTagInList(src + nameStart, nameLen, SAFE_WHITELIST_TAGS, SAFE_WHITELIST_TAGS_COUNT)) {
                // Strip unknown tag boundaries
                i = tagEnd;
                continue;
            }
        }

        // Clean attributes (remove on* and javascript:)
        std::string rawTag(src + tagStart, tagEnd - tagStart);
        std::string cleanedTag;
        cleanedTag.reserve(rawTag.size());

        size_t tk = 0;
        while (tk < rawTag.size()) {
            if (tk + 2 < rawTag.size() && (rawTag[tk] == ' ' || rawTag[tk] == '\t' || rawTag[tk] == '\n') &&
                (rawTag[tk+1] == 'o' || rawTag[tk+1] == 'O') && (rawTag[tk+2] == 'n' || rawTag[tk+2] == 'N')) {
                // Skip attribute name
                tk += 3;
                while (tk < rawTag.size() && rawTag[tk] != '=' && rawTag[tk] != ' ' && rawTag[tk] != '>') {
                    tk++;
                }
                if (tk < rawTag.size() && rawTag[tk] == '=') {
                    tk++;
                    if (tk < rawTag.size() && (rawTag[tk] == '"' || rawTag[tk] == '\'')) {
                        char q = rawTag[tk++];
                        while (tk < rawTag.size() && rawTag[tk] != q) tk++;
                        if (tk < rawTag.size()) tk++;
                    } else {
                        while (tk < rawTag.size() && rawTag[tk] != ' ' && rawTag[tk] != '>') tk++;
                    }
                }
            } else if (tk + 11 <= rawTag.size() && iequals(rawTag.c_str() + tk, 11, "javascript:", 11)) {
                while (tk < rawTag.size() && rawTag[tk] != '"' && rawTag[tk] != '\'' && rawTag[tk] != ' ' && rawTag[tk] != '>') {
                    tk++;
                }
                cleanedTag += "#";
            } else {
                cleanedTag += rawTag[tk++];
            }
        }

        out += cleanedTag;
        i = tagEnd;
    }

    return out;
}

JNIEXPORT jstring JNICALL Java_fasthtml_FastHTMLImpl_nativeSanitize
  (JNIEnv *env, jobject obj, jbyteArray htmlArray, jint safetyMode) {
    if (!htmlArray) return env->NewStringUTF("");

    jsize len = env->GetArrayLength(htmlArray);
    if (len <= 0) return env->NewStringUTF("");

    jbyte* bytes = env->GetByteArrayElements(htmlArray, NULL);
    std::string sanitized = sanitizeRawMemory((const char*)bytes, (size_t)len, safetyMode);
    env->ReleaseByteArrayElements(htmlArray, bytes, JNI_ABORT);

    return env->NewStringUTF(sanitized.c_str());
}

JNIEXPORT jstring JNICALL Java_fasthtml_FastHTMLImpl_nativeSanitizeAddress
  (JNIEnv *env, jobject obj, jlong address, jlong length, jint safetyMode) {
    if (address == 0 || length <= 0) return env->NewStringUTF("");

    const char* src = (const char*)address;
    std::string sanitized = sanitizeRawMemory(src, (size_t)length, safetyMode);

    return env->NewStringUTF(sanitized.c_str());
}

JNIEXPORT jobjectArray JNICALL Java_fasthtml_FastHTMLImpl_nativeTokenize
  (JNIEnv *env, jobject obj, jbyteArray htmlArray) {
    if (!htmlArray) {
        jclass strClass = env->FindClass("java/lang/String");
        return env->NewObjectArray(0, strClass, NULL);
    }

    jsize len = env->GetArrayLength(htmlArray);
    if (len <= 0) {
        jclass strClass = env->FindClass("java/lang/String");
        return env->NewObjectArray(0, strClass, NULL);
    }

    jbyte* bytes = env->GetByteArrayElements(htmlArray, NULL);
    const char* src = (const char*)bytes;

    std::vector<std::string> tokens;
    tokens.reserve(len / 16);

    size_t i = 0;
    while (i < (size_t)len) {
        if (src[i] == '<') {
            size_t j = i + 1;
            while (j < (size_t)len && src[j] != '>') {
                j++;
            }
            if (j < (size_t)len && src[j] == '>') {
                tokens.push_back(std::string(src + i, (j + 1) - i));
                i = j + 1;
            } else {
                tokens.push_back(std::string(src + i, len - i));
                i = len;
            }
        } else {
            size_t textStart = i;
            size_t textEnd = i;
            while (textEnd < (size_t)len && src[textEnd] != '<') {
                textEnd++;
            }
            if (textEnd > textStart) {
                tokens.push_back(std::string(src + textStart, textEnd - textStart));
            }
            i = textEnd;
        }
    }

    env->ReleaseByteArrayElements(htmlArray, bytes, JNI_ABORT);

    jclass strClass = env->FindClass("java/lang/String");
    jobjectArray result = env->NewObjectArray((jsize)tokens.size(), strClass, NULL);
    for (size_t idx = 0; idx < tokens.size(); idx++) {
        jstring js = env->NewStringUTF(tokens[idx].c_str());
        env->SetObjectArrayElement(result, (jsize)idx, js);
        env->DeleteLocalRef(js);
    }

    return result;
}
