#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>
#include "audiere.h"
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


typedef unsigned char u8;

int Min(int a, int b) {
  return (a < b ? a : b);
}


class ToneBuffer : public RefImplementation<SampleSource> {
public:
  ToneBuffer(double frequency) {
    RefPtr<SampleSource> tone(CreateTone(frequency));
    tone->getFormat(m_channel_count, m_sample_rate, m_sample_format);

    m_block_size = m_channel_count * GetSampleSize(m_sample_format);

    m_position = 0;
    m_buffer_length = 88200;
    m_buffer = new u8[m_block_size * m_buffer_length];
    memset(m_buffer, 0, m_block_size * m_buffer_length);
    tone->read(m_buffer_length, m_buffer);
  }

  ~ToneBuffer() {
    delete[] m_buffer;
  }

  void getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
  }

  int read(int sample_count, void* samples) {
    int to_read = Min(sample_count, m_buffer_length - m_position);
    memcpy(
      samples,
      m_buffer + m_position * m_block_size,
      to_read * m_block_size);
    m_position += to_read;
    return to_read;
  }

  void reset() {
    setPosition(0);
  }

  bool isSeekable() {
    return true;
  }

  int getLength() {
    return m_buffer_length;
  }

  void setPosition(int position) {
    m_position = position;
  }

  int getPosition() {
    return m_position;
  }

private:
  int m_channel_count;
  int m_sample_rate;
  SampleFormat m_sample_format;
  int m_block_size; // m_channel_count * GetSampleSize(m_sample_format)

  u8* m_buffer;
  int m_buffer_length;  // in samples
  int m_position;       // in samples
};


int main(int argc, char** argv) {
  std::string device_name;
  if (argc == 2) {
    device_name = argv[1];
  }

  RefPtr<AudioDevice> device(OpenDevice(device_name.c_str()));
  if (!device.get()) {
    cerr << "Opening output device failed" << endl;
    return EXIT_FAILURE;
  }

  RefPtr<OutputStream> s1(OpenSound(device.get(), new ToneBuffer(256)));
  RefPtr<OutputStream> s2(OpenSound(device.get(), new ToneBuffer(512)));
  RefPtr<OutputStream> s3(OpenSound(device.get(), new ToneBuffer(513)));
  RefPtr<OutputStream> s4(OpenSound(device.get(), new ToneBuffer(514)));

  if (!s1.get() || !s2.get() || !s3.get() || !s4.get()) {
    cerr << "OpenSound() failed" << endl;
    return EXIT_FAILURE;
  }

  s1->play();
  s2->play();
  s3->play();
  s4->play();

  // wait for five seconds
  PassOut(5);

  return EXIT_SUCCESS; // VC++ 6 sucks
}
