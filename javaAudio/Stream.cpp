#include <audiere.h>
#include "org/aegisknight/audiere/org_aegisknight_audiere_Stream.h"


////////////////////////////////////////////////////////////////////////////////

inline ADR_STREAM GetStream(JNIEnv* env, jobject obj)
{
  // get the audiere.Stream class
  jclass cls = env->GetObjectClass(obj);
  if (!cls) {
    return NULL;
  }

  jfieldID fid = env->GetFieldID(cls, "internal", "J");
  if (!fid) {
    return NULL;
  }

  return (ADR_STREAM)env->GetLongField(obj, fid);
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Stream_finalize(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    AdrCloseStream(stream);
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Stream_play(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    AdrPlayStream(stream);
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Stream_pause(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    AdrPauseStream(stream);
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Stream_reset(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    AdrResetStream(stream);
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jboolean JNICALL Java_org_aegisknight_audiere_Stream_isPlaying(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    return (AdrIsStreamPlaying(stream) ? JNI_TRUE : JNI_FALSE);
  } else {
    return JNI_FALSE;
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jboolean JNICALL Java_org_aegisknight_audiere_Stream_getRepeating(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    return (AdrGetStreamRepeat(stream) ? JNI_TRUE : JNI_FALSE);
  } else {
    return JNI_FALSE;
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Stream_setRepeating(
  JNIEnv* env,
  jobject obj,
  jboolean repeating)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    AdrSetStreamRepeat(stream, repeating ? ADR_TRUE : ADR_FALSE);
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_org_aegisknight_audiere_Stream_getVolume(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    return AdrGetStreamVolume(stream);
  } else {
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Stream_setVolume(
  JNIEnv* env,
  jobject obj,
  jint volume)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    AdrSetStreamVolume(stream, volume);
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_org_aegisknight_audiere_Stream_getPan(
  JNIEnv* env,
  jobject obj)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    return AdrGetStreamPan(stream);
  } else {
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Stream_setPan(
  JNIEnv* env,
  jobject obj,
  jint pan)
{
  ADR_STREAM stream = GetStream(env, obj);
  if (stream) {
    AdrSetStreamPan(stream, pan);
  }
}

////////////////////////////////////////////////////////////////////////////////
