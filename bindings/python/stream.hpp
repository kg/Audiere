#ifndef PYAUDIO_STREAM_HPP
#define PYAUDIO_STREAM_HPP


#include <Python.h>
#include <audiere.h>


extern PyTypeObject audiere_StreamType;


struct audiere_StreamObject {
  PyObject_HEAD
  ADR_STREAM stream;
};


#endif
