#ifndef PYAUDIO_CONTEXT_HPP
#define PYAUDIO_CONTEXT_HPP


#include <Python.h>
#include <audiere.h>


extern PyTypeObject audiere_ContextType;


struct audiere_ContextObject {
  PyObject_HEAD
  ADR_CONTEXT context;
};


#endif
