#include "AudioDevice.h"
#include "OutputStream.h"

void AudioDevice_dealloc(PyObject* self) {
  ((audiere_AudioDeviceObject*)self)->device->unref();
  PyObject_Del(self);
}


PyObject* NewPyOutputStream(audiere::OutputStream* stream) {
  audiere_OutputStreamObject* object = PyObject_New(
    audiere_OutputStreamObject, &audiere_OutputStreamType);
  if (!object) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create Python object");
    return 0;
  }

  object->stream = stream;
  object->stream->ref();
  return (PyObject*)object;
}


PyObject* AudioDevice_CreateTone(PyObject* self, PyObject* args) {
  audiere_AudioDeviceObject* object = (audiere_AudioDeviceObject*)self;

  double frequency;
  if (!PyArg_ParseTuple(args, "d", &frequency)) {
    return 0;
  }  

  audiere::RefPtr<audiere::OutputStream> stream(
    object->device->openStream(audiere::CreateTone(frequency)));
  if (!stream) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create tone");
    return 0;
  }

  return NewPyOutputStream(stream.get());
}


PyObject* AudioDevice_OpenSound(PyObject* self, PyObject* args) {
  audiere_AudioDeviceObject* object = (audiere_AudioDeviceObject*)self;
  
  // parse arguments
  const char* filename;
  int streaming = 0;
  if (!PyArg_ParseTuple(args, "s|i", &filename, &streaming)) {
    return 0;
  }

  audiere::RefPtr<audiere::OutputStream> stream(
    audiere::OpenSound(object->device, filename, streaming != 0));
  if (!stream) {
    PyErr_SetString(PyExc_RuntimeError, "Could not open sound");
    return 0;
  }

  return NewPyOutputStream(stream.get());
}


PyMethodDef AudioDeviceMethods[] = {
  { "create_tone", AudioDevice_CreateTone, METH_VARARGS },
  { "open_sound",  AudioDevice_OpenSound,  METH_VARARGS },
  { NULL, NULL }
};


PyObject* AudioDevice_getattr(PyObject* self, char* name) {
  return Py_FindMethod(AudioDeviceMethods, self, name);
}


PyTypeObject audiere_AudioDeviceType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "AudioDevice",
  sizeof(audiere_AudioDeviceObject),
  0,
  AudioDevice_dealloc,
  0,
  AudioDevice_getattr,
  0,   // setattr
};
