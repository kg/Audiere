#include "AudioDevice.h"
#include "OutputStream.h"


PyObject* Audiere_GetVersion(PyObject* /*self*/, PyObject* /*args*/) {
  return PyString_FromString(audiere::GetVersion());
}


PyObject* Audiere_OpenDevice(PyObject* /*self*/, PyObject* args) {
  const char* name = 0;
  const char* parameters = 0;

  if (!PyArg_ParseTuple(args, "|ss", &name, &parameters)) {
    return 0;
  }

  audiere::RefPtr<audiere::AudioDevice> device(
    audiere::OpenDevice(name, parameters));
  if (!device) {
    // throw an exception
    PyErr_SetString(PyExc_RuntimeError, "Could not open audio device");
    return 0;
  }

  // create and return python object
  audiere_AudioDeviceObject* object = PyObject_New(
    audiere_AudioDeviceObject, &audiere_AudioDeviceType);
  if (!object) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create Python object");
    return 0;
  }

  object->device = device.get();
  object->device->ref();
  return (PyObject*)object;
}


// module methods
PyMethodDef AudiereMethods[] = {
  { "get_version", Audiere_GetVersion, METH_VARARGS },
  { "open_device", Audiere_OpenDevice, METH_VARARGS },
  { 0, 0 }
};


#ifdef WIN32
  #define STDCALL __stdcall
#else
  #define STDCALL
#endif

extern "C" void STDCALL initaudiere() {
  audiere_AudioDeviceType.ob_type  = &PyType_Type;
  audiere_OutputStreamType.ob_type = &PyType_Type;
  Py_InitModule("audiere", AudiereMethods);
}
