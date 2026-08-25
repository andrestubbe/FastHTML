#ifndef FASTHTML_H
#define FASTHTML_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     fasthtml_FastHTMLImpl
 * Method:    nativeSanitize
 * Signature: ([BI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_fasthtml_FastHTMLImpl_nativeSanitize
  (JNIEnv *, jobject, jbyteArray, jint);

/*
 * Class:     fasthtml_FastHTMLImpl
 * Method:    nativeSanitizeAddress
 * Signature: (JJI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_fasthtml_FastHTMLImpl_nativeSanitizeAddress
  (JNIEnv *, jobject, jlong, jlong, jint);

/*
 * Class:     fasthtml_FastHTMLImpl
 * Method:    nativeTokenize
 * Signature: ([B)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_fasthtml_FastHTMLImpl_nativeTokenize
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     fasthtml_FastHTMLImpl
 * Method:    nativeHasAVX2
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_fasthtml_FastHTMLImpl_nativeHasAVX2
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif

#endif // FASTHTML_H
