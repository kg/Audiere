#include <stdio.h>
#include <audiere.h>
#include "org_aegisknight_audiere_OutputStream.h"
using namespace audiere;


OutputStream* GetStream(JNIEnv* env, jobject obj) {
  // get the audiere.OutputStream class
  jclass cls = env->GetObjectClass(obj);
  if (!cls) {
    return NULL;
  }

  jfieldID fid = env->GetFieldID(cls, "internal", "J");
  if (!fid) {
    return NULL;
  }

  return (OutputStream*)env->GetLongField(obj, fid);
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_finalize(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->unref();
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_play(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->play();
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_stop(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->stop();
  }
}


JNIEXPORT jboolean JNICALL Java_org_aegisknight_audiere_OutputStream_isPlaying(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    return (stream->isPlaying() ? JNI_TRUE : JNI_FALSE);
  } else {
    return JNI_FALSE;
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_reset(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->reset();
  }
}


JNIEXPORT jboolean JNICALL Java_org_aegisknight_audiere_OutputStream_getRepeating(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    return (stream->getRepeat() ? JNI_TRUE : JNI_FALSE);
  } else {
    return JNI_FALSE;
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_setRepeating(
  JNIEnv* env,
  jobject obj,
  jboolean repeating)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->setRepeat(repeating == JNI_TRUE);
  }
}


JNIEXPORT jfloat JNICALL Java_org_aegisknight_audiere_OutputStream_getVolume(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    return stream->getVolume();
  } else {
    return 0;
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_setVolume(
  JNIEnv* env,
  jobject obj,
  jfloat volume)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->setVolume(volume);
  }
}


JNIEXPORT jfloat JNICALL Java_org_aegisknight_audiere_OutputStream_getPan(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    return stream->getPan();
  } else {
    return 0;
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_setPan(
  JNIEnv* env,
  jobject obj,
  jfloat pan)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->setPan(pan);
  }
}


JNIEXPORT jboolean JNICALL Java_org_aegisknight_audiere_OutputStream_isSeekable(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    return (stream->isSeekable() ? JNI_TRUE : JNI_FALSE);
  } else {
    return false;
  }
}


JNIEXPORT jint JNICALL Java_org_aegisknight_audiere_OutputStream_getLength(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    return stream->getLength();
  } else {
    return 0;
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_OutputStream_setPosition(
  JNIEnv* env,
  jobject obj,
  jint position)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    stream->setPosition(position);
  }
}


JNIEXPORT jint JNICALL Java_org_aegisknight_audiere_OutputStream_getPosition(
  JNIEnv* env,
  jobject obj)
{
  OutputStream* stream = GetStream(env, obj);
  if (stream) {
    return stream->getPosition();
  } else {
    return 0;
  }
}
