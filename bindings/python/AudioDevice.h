#ifndef PY_AUDIO_DEVICE_H
#define PY_AUDIO_DEVICE_H


#include <Python.h>
#include <audiere.h>


extern PyTypeObject audiere_AudioDeviceType;


struct audiere_AudioDeviceObject {
  PyObject_HEAD
  audiere::AudioDevice* device;
};


#endif
