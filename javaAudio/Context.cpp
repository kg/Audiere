#include <audiere.h>
#include "org/aegisknight/audiere/org_aegisknight_audiere_Context.h"


////////////////////////////////////////////////////////////////////////////////

inline char* JavaStringToAscii(JNIEnv* env, jstring in)
{
  int length = env->GetStringLength(in);
  const jchar* chars = env->GetStringChars(in, NULL);

  if (!chars) {
    length = 0;
  }

  char* output = new char[length + 1];
  output[length] = 0;

  // convert string to ASCII
  for (int i = 0; i < length; i++) {
    if (chars[i] < 128) {
      output[i] = (char)chars[i];
    } else {
      output[i] = '?';
    }
  }

  if (chars) {
    env->ReleaseStringChars(in, chars);
  }

  return output;
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Context_initialize(
  JNIEnv* env,
  jobject obj,
  jstring output_device,
  jstring parameters)
{
  char* ascii_output_device = JavaStringToAscii(env, output_device);
  char* ascii_parameters    = JavaStringToAscii(env, parameters);

  // create the Audiere context
  ADR_CONTEXT context = AdrCreateContext(
    ascii_output_device,
    ascii_parameters,
    NULL, NULL, NULL, NULL, NULL, NULL
  );

  // we no longer need the ASCII versions of the strings
  delete[] ascii_output_device;
  delete[] ascii_parameters;

  // if context creation failed, throw an exception
  if (!context) {

    // get the exception class
    jclass create_context_exception =
      env->FindClass("org/aegisknight/audiere/CreateContextException");
    if (!create_context_exception) {
      return;  // give up
    }

    env->ThrowNew(create_context_exception, "Could not create Audiere context");
  }

  // otherwise, just attach the context to the object

  // The JNI *sucks* in that we can't attach an opaque pointer to an object.
  // Therefore, we just use a private long to store the pointer...
  // 64-bits should be enough.  God this feels dirty.

  jclass cls = env->GetObjectClass(obj);
  if (!cls) {
    return;
  }

  jfieldID fid = env->GetFieldID(cls, "internal", "J");
  if (!fid) {
    return;
  }

  env->SetLongField(obj, fid, (jlong)context);
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_org_aegisknight_audiere_Context_finalize(
  JNIEnv* env,
  jobject obj)
{
  // retrieve the Audiere context
  jclass cls = env->GetObjectClass(obj);
  if (!cls) {
    return;
  }

  jfieldID fid = env->GetFieldID(cls, "internal", "J");
  if (!fid) {
    return;
  }

  ADR_CONTEXT context = (ADR_CONTEXT)env->GetLongField(obj, fid);
  if (context) {
    AdrDestroyContext(context);
  }
}

////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jobject JNICALL Java_org_aegisknight_audiere_Context_openStream(
  JNIEnv* env,
  jobject obj,
  jstring filename)
{
  char* ascii_filename = JavaStringToAscii(env, filename);

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

  ADR_CONTEXT context = (ADR_CONTEXT)env->GetLongField(obj, fid);
  if (context) {

    // attempt to open the stream
    ADR_STREAM stream = AdrOpenStream(context, ascii_filename);
    if (stream) {

      jclass clazz = env->FindClass("org/aegisknight/audiere/Stream");
      if (clazz) {
        // now try to create an org.aegisknight.audiere.Stream
        rv = env->AllocObject(clazz);
        if (rv) {
          jfieldID sid = env->GetFieldID(clazz, "internal", "J");
          if (sid) {
            env->SetLongField(rv, sid, (jlong)stream);
          } else {
            rv = NULL;
          }
        }
      }

    }

  }

  // if we didn't open a stream, throw an exception
  if (rv == NULL) {
    // get the exception class
    jclass open_stream_exception =
      env->FindClass("org/aegisknight/audiere/OpenStreamException");
    if (!open_stream_exception) {
      return NULL;  // give up
    }

    env->ThrowNew(open_stream_exception, "Could not open Audiere stream");
  }

  return rv;
}

////////////////////////////////////////////////////////////////////////////////
