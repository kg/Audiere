// these tests aren't really interactive yet

#include <iostream>
#include <memory>
#include "audiere.h"
using namespace std;
using namespace audiere;


#ifdef WIN32

#include <windows.h>
void passOut(int milliseconds) {
  Sleep(milliseconds);
}

#else  // assume POSIX

#include <unistd.h>
void passOut(int milliseconds) {
  int seconds = milliseconds / 1000;
  int useconds = (milliseconds % 1000) * 1000;
  sleep(seconds);
  usleep(useconds);
}

#endif


void testDriver(const char* driver) {
  cout << "testDriver " << driver << "\n--" << endl;

  ContextAttr attr;
  attr.setOutputDevice(driver);
  auto_ptr<Context> context(CreateContext(&attr));
  if (!context.get()) {
    cout << "Error creating output context" << endl;
    return;
  }

  auto_ptr<Stream> stream1(context->openStream("laugh.wav"));
  if (!stream1.get()) {
    cout << "Error opening laugh.wav" << endl;
    return;
  }

  auto_ptr<Stream> stream2(context->openStream("shot.wav"));
  if (!stream2.get()) {
    cout << "Error opening shot.wav" << endl;
    return;
  }

  for (int i = 0; i < 3; ++i) {
    cout << "Playing laugh.wav...  this should last about one second..." << endl;
    stream1->play();
    while (stream1->isPlaying()) {
      passOut(50);
    }
    cout << "Done\n" << endl;

    cout << "Playing shot.wav...  this should last about one second..." << endl;
    stream2->play();
    while (stream2->isPlaying()) {
      passOut(50);
    }
    cout << "Done\n" << endl;
  }
}


int main() {
  testDriver("null");
  testDriver("");  // default

  // VC++ 6 sucks
  return 0;
}
