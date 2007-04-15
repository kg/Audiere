#include <assert.h>
#include <windows.h>
#include "audiere.h"
using namespace std;
using namespace audiere;


int main() {
    AudioDevicePtr device(OpenDevice());
    assert(device);

    std::vector<OutputStreamPtr> streams;

    while (true) {
        OutputStreamPtr sound = OpenSound(device, "data/laugh.wav");
        assert(sound);
        sound->play();
        streams.push_back(sound);
        if (streams.size() > 100) {
            streams.erase(streams.begin());
        }
        Sleep(250);
    }
}
