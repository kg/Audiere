#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <audiere.h>
using namespace std;
using namespace audiere;


#ifdef WIN32

#include <windows.h>
inline void sleepSecond() {
  Sleep(1000);
}

#else  // assume POSIX

#include <unistd.h>
inline void sleepSecond() {
  sleep(1);
}

#endif


int main(int argc, char** argv) {

  if (argc != 2) {
    cerr << "usage: simple <filename>" << endl;
    return EXIT_FAILURE;
  }

  cerr << "initializing..." << endl;

  RefPtr<AudioDevice> device(OpenDevice());
  if (!device.get()) {
    cerr << "OpenDevice() failed" << endl;
    return EXIT_FAILURE;
  }

  cerr << "created context" << endl;

  RefPtr<OutputStream> sound(OpenSound(device.get(), argv[1]));
  if (!sound.get()) {
    cerr << "OpenSound() failed" << endl;
    return EXIT_FAILURE;
  }

  cerr << "opened sound" << endl;

  sound->play();

  cerr << "started playback" << endl;
  while (sound->isPlaying()) {
    sleepSecond();
  }
}
