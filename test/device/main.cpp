#include <iostream>
#include <memory>
#include <string>
#include <math.h>
#include <stdlib.h>
#include "audiere.h"
#include "utility.h"
#include "types.h"
using namespace std;
using namespace audiere;


#ifdef WIN32

  #include <windows.h>
  void PassOut(int seconds) {
    Sleep(seconds * 1000);
  }

#else

  #include <unistd.h>
  void PassOut(int seconds) {
    sleep(seconds);
  }

#endif


static const double PI = 3.14159265358979323846;


// @todo  make this class endian-independent
class SineWave : public UnseekableSource {
public:
  SineWave(int frequency) {
    m_frequency = (frequency == 0 ? 1 : frequency);
    doReset(); // not supposed to call virtual functions in constructors
  }

  void getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = 1;
    sample_rate   = 44100;
    sample_format = SF_S16_LE;
  }

  int read(int sample_count, void* buffer) {
    s16* out = (s16*)buffer;
    for (int i = 0; i < sample_count; ++i) {
      double h = sin(2 * PI * m_frequency / 44100 * elapsed++);
      out[i] = normal_to_s16(h);
    }

    return sample_count;
  }

  void reset() {
    doReset();
  }

private:
  void doReset() {
    elapsed = 0;
  }

  s16 normal_to_s16(double d) {
    d = (d + 1) / 2; // convert from [-1, 1] to [0, 1]
    return d * 32767 - 16384;
  }

  int m_frequency;
  long elapsed;
};


int main(int argc, char** argv) {
  std::string device_name;
  if (argc == 2) {
    device_name = argv[1];
  }

  auto_ptr<AudioDevice> device(OpenDevice(device_name.c_str()));
  if (!device.get()) {
    cerr << "Opening output device failed" << endl;
    return EXIT_FAILURE;
  }

  auto_ptr<OutputStream> stream1(device->openStream(new SineWave(511)));
  auto_ptr<OutputStream> stream2(device->openStream(new SineWave(512)));

  stream1->play();
  stream2->play();

  // wait for five seconds
  PassOut(1);

  return EXIT_SUCCESS; // VC++ 6 sucks
}
