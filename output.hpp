#ifndef OUTPUT_HPP
#define OUTPUT_HPP


// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <utility>
#include <list>
#include "data_types.hpp"
#include "audiere.h"


// types

typedef int (ADR_CALL *ADR_SAMPLE_SOURCE)(
  void* opaque,
  int sample_count,
  void* samples);

typedef void (ADR_CALL *ADR_SAMPLE_RESET)(
  void* opaque);


class IOutputStream;

class IOutputContext
{
public:
  virtual ~IOutputContext() { }

  virtual bool Initialize(const char* parameters) = 0;

  virtual void Update() = 0;

  virtual IOutputStream* OpenStream(
    int channel_count,
    int sample_rate,
    int bits_per_sample,
    ADR_SAMPLE_SOURCE source,
    ADR_SAMPLE_RESET reset,
    void* opaque
  ) = 0;
};

class IOutputStream
{
public:
  virtual ~IOutputStream() { }

  virtual void Play() = 0;
  virtual void Stop() = 0;
  virtual void Reset() = 0;
  virtual bool IsPlaying() = 0;
  virtual void SetVolume(int volume) = 0;
  virtual int  GetVolume() = 0;
  virtual void SetPan(int pan) = 0;
  virtual int  GetPan() = 0;
};


// functions

extern IOutputContext* OpenContext(
  const char* output_device,
  const char* parameters);

// helper function to parse parameters
typedef std::pair<adr::string, adr::string> Parameter;
typedef std::list<Parameter> ParameterList;

extern void ParseParameters(
  const char* parameter_string,
  ParameterList& parameters);


#endif
