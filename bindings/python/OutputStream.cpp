#include "OutputStream.h"


void OutputStream_dealloc(PyObject* self) {
  ((audiere_OutputStreamObject*)self)->stream->unref();
  PyObject_Del(self);
}


static PyObject* OutputStream_Reset(PyObject* self, PyObject* /*args*/) {
  audiere_OutputStreamObject* object = (audiere_OutputStreamObject*)self;

  object->stream->reset();
  return Py_None;
}


static PyMethodDef OutputStreamMethods[] = {
  { "reset", OutputStream_Reset, METH_VARARGS },
  { 0, 0 },
};


static PyObject* OutputStream_getattr(PyObject* self, char* name) {
  audiere_OutputStreamObject* object = (audiere_OutputStreamObject*)self;

  if (strcmp(name, "playing") == 0) {
    return PyInt_FromLong(object->stream->isPlaying());    
  } else if (strcmp(name, "repeating") == 0) {
    return PyInt_FromLong(object->stream->getRepeat());
  } else if (strcmp(name, "volume") == 0) {
    return PyFloat_FromDouble(object->stream->getVolume());
  } else if (strcmp(name, "pan") == 0) {
    return PyFloat_FromDouble(object->stream->getPan());
  } else if (strcmp(name, "seekable") == 0) {
    return PyInt_FromLong(object->stream->isSeekable());
  } else if (strcmp(name, "length") == 0) {
    return PyInt_FromLong(object->stream->getLength());
  } else if (strcmp(name, "position") == 0) {
    return PyInt_FromLong(object->stream->getPosition());
  } else if (strcmp(name, "pitchshift") == 0) {
    return PyFloat_FromDouble(object->stream->getPitchShift());
  } else {
    return Py_FindMethod(OutputStreamMethods, self, name);
  }
}


static int OutputStream_setattr(PyObject* self, char* name, PyObject* value) {
  audiere_OutputStreamObject* object = (audiere_OutputStreamObject*)self;

  if (strcmp(name, "playing") == 0) {
    if (PyInt_AsLong(value)) {
      object->stream->play();
    } else {
      object->stream->stop();
    }
  } else if (strcmp(name, "repeating") == 0) {
    object->stream->setRepeat(PyInt_AsLong(value) != 0);
  } else if (strcmp(name, "volume") == 0) {
    object->stream->setVolume((float)PyFloat_AsDouble(value));
  } else if (strcmp(name, "pan") == 0) {
    object->stream->setPan((float)PyFloat_AsDouble(value));
  } else if (strcmp(name, "pitchshift") == 0) {
    object->stream->setPitchShift((float)PyFloat_AsDouble(value));
  } else if (strcmp(name, "position") == 0) {
    object->stream->setPosition(PyInt_AsLong(value));
  } else if (strcmp(name, "seekable") == 0 ||
             strcmp(name, "length") == 0)
  {
    PyErr_SetString(PyExc_RuntimeError, "Cannot modify read-only attribute");
    return -1;
  }

  return 0;
}


PyTypeObject audiere_OutputStreamType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "OutputStream",
  sizeof(audiere_OutputStreamObject),
  0,
  OutputStream_dealloc,
  0,
  OutputStream_getattr,
  OutputStream_setattr,
};
