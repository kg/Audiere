#include "stream.hpp"


////////////////////////////////////////////////////////////////////////////////

static void Stream_dealloc(PyObject* self)
{
  audiere_StreamObject* object = (audiere_StreamObject*)self;
  AdrCloseStream(object->stream);

  PyObject_Del(self);
}

////////////////////////////////////////////////////////////////////////////////

static PyObject* Stream_Play(PyObject* self, PyObject* args)
{
  audiere_StreamObject* object = (audiere_StreamObject*)self;

  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  AdrPlayStream(object->stream);
  return Py_None;
}

////////////////////////////////////////////////////////////////////////////////

static PyObject* Stream_Pause(PyObject* self, PyObject* args)
{
  audiere_StreamObject* object = (audiere_StreamObject*)self;

  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  AdrPauseStream(object->stream);
  return Py_None;
}

////////////////////////////////////////////////////////////////////////////////

static PyObject* Stream_Reset(PyObject* self, PyObject* args)
{
  audiere_StreamObject* object = (audiere_StreamObject*)self;

  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  AdrResetStream(object->stream);
  return Py_None;
}

////////////////////////////////////////////////////////////////////////////////

static PyMethodDef StreamMethods[] = {
  { "play",  Stream_Play,  METH_VARARGS },
  { "pause", Stream_Pause, METH_VARARGS },
  { "reset", Stream_Reset, METH_VARARGS },
  { NULL, NULL },
};

////////////////////////////////////////////////////////////////////////////////

static PyObject* Stream_getattr(PyObject* self, char* name)
{
  audiere_StreamObject* object = (audiere_StreamObject*)self;

  if (strcmp(name, "volume") == 0) {

    return PyInt_FromLong(AdrGetStreamVolume(object->stream));
      
  } else if (strcmp(name, "repeating") == 0) {

    bool repeating = AdrGetStreamRepeat(object->stream);
    return PyInt_FromLong(repeating ? 1 : 0);

  } else if (strcmp(name, "playing") == 0) {

    bool playing = AdrIsStreamPlaying(object->stream);
    return PyInt_FromLong(playing ? 1 : 0);

  } else if (strcmp(name, "VOLUME_MIN") == 0) {
    return PyInt_FromLong(ADR_VOLUME_MIN);
  } else if (strcmp(name, "VOLUME_MAX") == 0) {
    return PyInt_FromLong(ADR_VOLUME_MAX);
  } else {
    return Py_FindMethod(StreamMethods, self, name);
  }
}

////////////////////////////////////////////////////////////////////////////////

static int Stream_setattr(PyObject* self, char* name, PyObject* value)
{
  audiere_StreamObject* object = (audiere_StreamObject*)self;

  if (strcmp(name, "volume") == 0) {

    AdrSetStreamVolume(object->stream, PyInt_AsLong(value));
    return 0;

  } else if (strcmp(name, "repeating") == 0) {
    
    AdrSetStreamRepeat(
      object->stream,
      PyObject_IsTrue(value) ? true : false);
    return 0;

  // check read-only attributes
  } else if (strcmp(name, "playing")    == 0 ||
             strcmp(name, "VOLUME_MIN") == 0 ||
             strcmp(name, "VOLUME_MAX") == 0) {

    PyErr_SetString(PyExc_RuntimeError, "cannot modify read-only attribute");
    return -1;  // read-only

  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

PyTypeObject audiere_StreamType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "Stream",
  sizeof(audiere_StreamObject),
  0,
  Stream_dealloc,
  0,
  Stream_getattr,
  Stream_setattr,
};

////////////////////////////////////////////////////////////////////////////////
