program aplayer;

uses
  Windows,
  SysUtils,
  Forms,
  audiere in 'audiere.pas';

(*
AudioDevicePtr device = OpenDevice();
OutputStreamPtr sound = OpenSound(device.get(), "filename.mp3");
sound->play();
*)

var
  Device: PAudioDevice;
  Sound : POutputStream;

begin
  MessageBox(0, AdrGetVersion, 'Info', MB_OK);
  Device := AdrOpenDevice('', '');
  Sound := AdrOpenSoundFromFile(Device, 'song1.ogg', True);
  Sound.Play;

  repeat
    Application.ProcessMessages;
  until Sound.IsPlaying = False;

  Sound.unref;
  Device.unref;
end.
