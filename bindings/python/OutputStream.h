#ifndef PYAUDIO_STREAM_HPP
#define PYAUDIO_STREAM_HPP


#include <Python.h>
#include <audiere.h>


extern PyTypeObject audiere_OutputStreamType;


struct audiere_OutputStreamObject {
  PyObject_HEAD
  audiere::RefPtr<audiere::OutputStream> stream;
};


#endif
