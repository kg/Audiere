#include "context.hpp"
#include "stream.hpp"


static PyObject* Audiere_OpenContext(PyObject* self, PyObject* args)
{
  const char* output_device;
  const char* parameters;
  if (!PyArg_ParseTuple(args, "ss", &output_device, &parameters)) {
    return NULL;
  }

  // create context
  ADR_CONTEXT context = AdrCreateContext(
    output_device,
    parameters,
    NULL, NULL, NULL, NULL, NULL, NULL
  );
  if (context) {

    // create and return python object
    audiere_ContextObject* object = PyObject_New(
      audiere_ContextObject,
      &audiere_ContextType
    );

    if (object) {
      object->context = context;
      return (PyObject*)object;
    } else {

      // throw an exception
      AdrDestroyContext(context);
      PyErr_SetString(PyExc_RuntimeError, "Could not create Python object, out of memory?");
      return NULL;
    }

  } else {

    // throw an exception
    PyErr_SetString(PyExc_RuntimeError, "Could not create Audiere context");
    return NULL;
  }
}


// module methods
static PyMethodDef AudiereMethods[] = {
  { "open_context", Audiere_OpenContext, METH_VARARGS },
  { NULL, NULL }
};


void __stdcall initaudiere()
{
  audiere_ContextType.ob_type = &PyType_Type;
  audiere_StreamType.ob_type  = &PyType_Type;
  Py_InitModule("audiere", AudiereMethods);
}
