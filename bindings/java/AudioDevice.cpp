#include <audiere.h>
#include <string>
#include "org_aegisknight_audiere_AudioDevice.h"
using namespace audiere;


std::string JavaStringToAscii(JNIEnv* env, jstring in) {
  int length = env->GetStringLength(in);
  const jchar* chars = env->GetStringChars(in, NULL);

  if (!chars) {
    return "";
  }

  std::string result(length, ' ');

  // convert string to ASCII
  for (int i = 0; i < length; i++) {
    result[i] = (chars[i] < 128 ? (char)chars[i] : '?');
  }

  env->ReleaseStringChars(in, chars);
  return result;
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_AudioDevice_initialize(
  JNIEnv* env,
  jobject obj,
  jstring name,
  jstring parameters)
{
  RefPtr<AudioDevice> device = OpenDevice(
    JavaStringToAscii(env, name).c_str(),
    JavaStringToAscii(env, parameters).c_str());
  if (!device) {
    // throw an exception

    jclass create_context_exception =
      env->FindClass("org/aegisknight/audiere/AudiereException");
    if (create_context_exception) {
      env->ThrowNew(create_context_exception, "Could not open audio device");
    }
    return;
  }

  // The JNI *sucks* in that we can't attach an opaque pointer to an object.
  // Therefore, we just use a private long to store the pointer...
  // 64-bits should be enough.  God this feels dirty.

  jclass cls = env->GetObjectClass(obj);
  if (cls) {
    jfieldID fid = env->GetFieldID(cls, "internal", "J");
    if (fid) {
      device->ref();
      env->SetLongField(obj, fid, (jlong)device.get());
    }
  }
}


JNIEXPORT void JNICALL Java_org_aegisknight_audiere_AudioDevice_finalize(
  JNIEnv* env,
  jobject obj)
{
  jclass cls = env->GetObjectClass(obj);
  if (cls) {
    jfieldID fid = env->GetFieldID(cls, "internal", "J");
    if (fid) {
      AudioDevice* device = (AudioDevice*)env->GetLongField(obj, fid);
      if (device) {
        device->unref();
      }
    }
  }
}


JNIEXPORT jobject JNICALL Java_org_aegisknight_audiere_AudioDevice_createTone(
  JNIEnv* env,
  jobject obj,
  jdouble frequency)
{
  // retrieve the Audiere context
  jclass cls = env->GetObjectClass(obj);
  if (!cls) {
    return NULL;
  }

  jfieldID fid = env->GetFieldID(cls, "internal", "J");
  if (!fid) {
    return NULL;
  }

  // the created object
  jobject rv = NULL;

  AudioDevice* device = (AudioDevice*)env->GetLongField(obj, fid);
  if (device) {

    // attempt to open the stream
    RefPtr<OutputStream> stream(device->openStream(CreateTone(frequency)));
    if (stream) {

      jclass clazz = env->FindClass("org/aegisknight/audiere/OutputStream");
      if (clazz) {
        // now try to create an org.aegisknight.audiere.OutputStream
        rv = env->AllocObject(clazz);
        if (rv) {
          jfieldID sid = env->GetFieldID(clazz, "internal", "J");
          if (sid) {
            stream->ref();
            env->SetLongField(rv, sid, (jlong)stream.get());
          } else {
            rv = NULL;
          }
        }
      }

    }

  }

  // if we didn't open a stream, throw an exception
  if (!rv) {
    // get the exception class
    jclass open_stream_exception =
      env->FindClass("org/aegisknight/audiere/AudiereException");
    if (!open_stream_exception) {
      return NULL;  // give up
    }

    env->ThrowNew(open_stream_exception, "Could not open Audiere stream");
  }

  return rv;
}


JNIEXPORT jobject JNICALL Java_org_aegisknight_audiere_AudioDevice_openSound(
  JNIEnv* env,
  jobject obj,
  jstring fn,
  jboolean streaming)
{
  std::string filename(JavaStringToAscii(env, fn));

  // retrieve the Audiere context
  jclass cls = env->GetObjectClass(obj);
  if (!cls) {
    return NULL;
  }

  jfieldID fid = env->GetFieldID(cls, "internal", "J");
  if (!fid) {
    return NULL;
  }

  // the created object
  jobject rv = NULL;

  AudioDevice* device = (AudioDevice*)env->GetLongField(obj, fid);
  if (device) {

    // attempt to open the stream
    RefPtr<OutputStream> stream(OpenSound(
      device,
      filename.c_str(),
      streaming == JNI_TRUE));
    if (stream) {

      jclass clazz = env->FindClass("org/aegisknight/audiere/OutputStream");
      if (clazz) {
        // now try to create an org.aegisknight.audiere.OutputStream
        rv = env->AllocObject(clazz);
        if (rv) {
          jfieldID sid = env->GetFieldID(clazz, "internal", "J");
          if (sid) {
            stream->ref();
            env->SetLongField(rv, sid, (jlong)stream.get());
          } else {
            rv = NULL;
          }
        }
      }

    }

  }

  // if we didn't open a stream, throw an exception
  if (!rv) {
    // get the exception class
    jclass open_stream_exception =
      env->FindClass("org/aegisknight/audiere/AudiereException");
    if (!open_stream_exception) {
      return NULL;  // give up
    }

    env->ThrowNew(open_stream_exception, "Could not open Audiere stream");
  }

  return rv;
}
