#include "context.hpp"
#include "stream.hpp"


////////////////////////////////////////////////////////////////////////////////

static void Context_dealloc(PyObject* self)
{
  audiere_ContextObject* object = (audiere_ContextObject*)self;
  AdrDestroyContext(object->context);

  PyObject_Del(self);
}

////////////////////////////////////////////////////////////////////////////////

static PyObject* Context_OpenStream(PyObject* self, PyObject* args)
{
  audiere_ContextObject* object = (audiere_ContextObject*)self;
  
  // parse arguments
  const char* filename;
  if (!PyArg_ParseTuple(args, "s", &filename)) {
    return NULL;
  }

  ADR_STREAM stream = AdrOpenStream(object->context, filename);
  if (stream) {
    
    // create the Python object
    audiere_StreamObject* object = PyObject_New(
      audiere_StreamObject,
      &audiere_StreamType
    );

    if (object) {

      object->stream = stream;
      return (PyObject*)object;
    
    } else {
      AdrCloseStream(stream);
      PyErr_SetString(PyExc_RuntimeError, "Could not create Python object, out of memory?");
      return NULL;
    }

  } else {
    PyErr_SetString(PyExc_RuntimeError, "Could not load song");
    return NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////

static PyMethodDef ContextMethods[] = {
  { "open_stream", Context_OpenStream, METH_VARARGS },
  { NULL, NULL }
};

////////////////////////////////////////////////////////////////////////////////

static PyObject* Context_getattr(PyObject* self, char* name)
{
  return Py_FindMethod(ContextMethods, self, name);
}

////////////////////////////////////////////////////////////////////////////////

PyTypeObject audiere_ContextType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "Context",
  sizeof(audiere_ContextObject),
  0,
  Context_dealloc,
  0,
  Context_getattr,
  0,   // setattr
};

////////////////////////////////////////////////////////////////////////////////
