//=========================================================================
// Audiere Sound System
// Version 1.9.2
// (c) 2002 Chad Austin
//
// This API uses principles explained at
// http://aegisknight.org/cppinterface.html
//
// This code licensed under the terms of the LGPL.  See the Audiere
// license.txt.
//-------------------------------------------------------------------------
// Delphi Conversion By:
// Jarrod Davis
// Jarrod Davis Software
// http://www.jarroddavis.com   - Jarrod Davis Software
// http://www.gamevisionsdk.com - Game Application Framework for Delphi
// support@jarroddavis.com      - Support Email
//-------------------------------------------------------------------------
// How to use:
//   * Include Audiere in your Uses statement
//   * Enable or Disable the DYNAMICS compiler define
//   * If Dynamic, be sure to call AdrLoadDLL before using any commands.
//     the DLL will be automaticlly unloaded at termination.
//   * If Static, then use as normal.
// History:
//   * Initial 1.9.2 release.
//   + Added dynamic loading. Use the DYNAMIC compiler define to control
//     this. When enabled you can then use ArdLoadLL/AdrUnloadDLL to
//     dynamiclly load/unload dll at runtime.
//=========================================================================

unit Audiere;

{$A+}
{$Z4}
{$DEFINE DYNAMIC}

interface

const
  DLL_NAME = 'Audiere.dll';

type

  { TAdrSeekMode  }
  TAdrSeekMode = (
    Adr_Seek_Begin,
    Adr_Seek_Current,
    Adr_Seek_End
  );

  { TAdrSoundEffectType }
  TAdrSoundEffectType = (
    Adr_SoundEffectType_Single,
    Adr_SoundEffectType_Multiple
  );

  { TAdrSampleFormat }
  TAdrSampleFormat = (
    Adr_SampleFormat_U8,
    Adr_SampleFormat_S16
  );

  { TAdrFileFormat }
  TAdrFileFormat = (
    FF_AUTODETECT,
    FF_WAV,
    FF_OGG,
    FF_FLAC,
    FF_MP3,
    FF_MOD
  );

  { TAdrRefCounted  }
  TAdrRefCounted = class
  public
    procedure Ref;   virtual; stdcall; abstract;
    procedure UnRef; virtual; stdcall; abstract;
  end;

  { TAdrFile }
  TAdrFile = class(TAdrRefCounted)
  public
    function Read(aBuffer: Pointer; aSize: Integer): Integer; virtual; stdcall; abstract;
    function Seek(aPosition: Integer; aSeekMode: TAdrSeekMode): Boolean; virtual; stdcall; abstract;
    function Tell: Integer; virtual; stdcall; abstract;
  end;

  { TAdrSampleSource }
  TAdrSampleSource = class(TAdrRefCounted)
  public
    procedure GetFormat(var aChannelCount: Integer; var aSampleRate: Integer; var aSampleFormat: TAdrSampleFormat); virtual; stdcall; abstract;
    function  Read(aFrameCount: Integer; aBuffer: Pointer): Integer;  virtual; stdcall; abstract;
    procedure Reset; virtual; stdcall; abstract;
    function  IsSeekable: Boolean; virtual; stdcall; abstract;
    function  GetLength: Integer; virtual; stdcall; abstract;
    procedure SetPosition(Position: Integer); virtual; stdcall; abstract;
    function  GetPosition: Integer; virtual; stdcall; abstract;
  end;

  { TAdrOutputStream }
  TAdrOutputStream = class(TAdrRefCounted)
  public
    procedure Play; virtual; stdcall; abstract;
    procedure Stop; virtual; stdcall; abstract;
    function  IsPlaying: Boolean; virtual; stdcall; abstract;
    procedure Reset; virtual; stdcall; abstract;
    procedure SetRepeat(aRepeat: Boolean); virtual; stdcall; abstract;
    function  GetRepeat: Boolean; virtual; stdcall; abstract;
    procedure SetVolume(aVolume: Single); virtual; stdcall; abstract;
    function  GetVolume: Single; virtual; stdcall; abstract;
    procedure SetPan(aPan: Single); virtual; stdcall; abstract;
    function  GetPan: Single; virtual; stdcall; abstract;
    procedure SetPitchShift(aShift: Single); virtual; stdcall; abstract;
    function  GetPitchShift: Single; virtual; stdcall; abstract;
    function  IsSeekable: Boolean; virtual; stdcall; abstract;
    function  GetLength: Integer; virtual; stdcall; abstract;
    procedure SetPosition(aPosition: Integer); virtual; stdcall; abstract;
    function  GetPosition: Integer; virtual; stdcall; abstract;
  end;

  { TAdrAudioDevice }
  TAdrAudioDevice = class(TAdrRefCounted)
  public
    procedure Update; virtual; stdcall; abstract;
    function  OpenStream(aSource: TAdrSampleSource): TAdrOutputStream; virtual; stdcall; abstract;
    function  OpenBuffer(aSamples: Pointer; aFrameCount, aChannelCount, aSampleRate: Integer; aSampelFormat: TAdrSampleFormat):  TAdrOutputStream; virtual; stdcall; abstract;
  end;

  { TAdrSampleBuffer }
  TAdrSampleBuffer = class(TAdrRefCounted)
  public
    procedure GetFormat(var ChannelCount: Integer; var aSampleRate: Integer; var aSampleFormat: TAdrSampleFormat); virtual; stdcall; abstract;
    function  GetLength: Integer; virtual; stdcall; abstract;
    function  GetSamples: Pointer; virtual; stdcall; abstract;
    function  OpenStream: TAdrSampleSource; virtual; stdcall; abstract;
  end;

  { TAdrSoundEffect }
  TAdrSoundEffect = class(TAdrRefCounted)
  public
    procedure Play; virtual; stdcall; abstract;
    procedure Stop; virtual; stdcall; abstract;
    procedure SetVolume(aVolume: Single); virtual; stdcall; abstract;
    function  GetVolume: Single; virtual; stdcall; abstract;
    procedure SetPan(aPan: Single); virtual; stdcall; abstract;
    function  GetPan: Single; virtual; stdcall; abstract;
    procedure SetPitchShift(aShift: Single); virtual; stdcall; abstract;
    function  GetPitchShift: Single; virtual; stdcall; abstract;
  end;

{ --- Audiere Routines -------------------------------------------------- }
{$IFNDEF DYNAMIC}
function AdrGetVersion: PChar; stdcall; external DLL_NAME name '_AdrGetVersion@0';
function AdrGetSupportedFileFormats: PChar; stdcall; external DLL_NAME name '_AdrGetSupportedFileFormats@0';
function AdrGetSupportedAudioDevices: PChar; stdcall; external DLL_NAME name '_AdrGetSupportedAudioDevices@0';
function AdrGetSampleSize(aFormat: TAdrSampleFormat): Integer; stdcall; external DLL_NAME name '_AdrGetSampleSize@4';
function AdrOpenDevice(const aName: PChar; const aParams: PChar): TAdrAudioDevice; stdcall; external DLL_NAME name '_AdrOpenDevice@8';
function AdrOpenSampleSource(const aFilename: PChar; aFileFormat: TAdrFileFormat): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrOpenSampleSource@8';
function AdrOpenSampleSourceFromFile(aFile: TAdrFile; aFileFormat: TAdrFileFormat): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrOpenSampleSourceFromFile@8';
function AdrCreateTone(aFrequency: Double): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreateTone@8';
function AdrCreateSquareWave(aFrequency: Double): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreateSquareWave@8';
function AdrCreateWhiteNoise: TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreateWhiteNoise@0';
function AdrCreatePinkNoise: TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreatePinkNoise@0';
function AdrOpenSound(aDevice: TAdrAudioDevice; aSource: TAdrSampleSource; aStreaming: LongBool): TAdrOutputStream; stdcall; external DLL_NAME name '_AdrOpenSound@12';
function AdrCreateSampleBuffer(aSamples: Pointer; aFrameCount, aChannelCount, aSampleRate: Integer; aSampleFormat: TAdrSampleFormat): TAdrSampleBuffer; stdcall; external DLL_NAME name '_AdrCreateSampleBuffer@20';
function AdrCreateSampleBufferFromSource(aSource: TAdrSampleSource): TAdrSampleBuffer; stdcall; external DLL_NAME name '_AdrCreateSampleBufferFromSource@4';
function AdrOpenSoundEffect(aDevice: TAdrAudioDevice; aSource: TAdrSampleSource; aType: TAdrSoundEffectType): TAdrSoundEffect; stdcall; external DLL_NAME name '_AdrOpenSoundEffect@12';
{$ENDIF}

{$IFDEF DYNAMIC}
var
  AdrGetVersion                  : function: PChar; stdcall = nil;
  AdrGetSupportedFileFormats     : function: PChar; stdcall = nil;
  AdrGetSupportedAudioDevices    : function : PChar; stdcall = nil;
  AdrGetSampleSize               : function(aFormat: TAdrSampleFormat): Integer; stdcall = nil;
  AdrOpenDevice                  : function(const aName: PChar; const aParams: PChar): TAdrAudioDevice; stdcall = nil;
  AdrOpenSampleSource            : function(const aFilename: PChar; aFileFormat: TAdrFileFormat): TAdrSampleSource; stdcall = nil;
  AdrOpenSampleSourceFromFile    : function(aFile: TAdrFile; aFileFormat: TAdrFileFormat): TAdrSampleSource; stdcall = nil;
  AdrCreateTone                  : function(aFrequency: Double): TAdrSampleSource; stdcall = nil;
  AdrCreateSquareWave            : function(aFrequency: Double): TAdrSampleSource; stdcall = nil;
  AdrCreateWhiteNoise            : function: TAdrSampleSource; stdcall = nil;
  AdrCreatePinkNoise             : function: TAdrSampleSource; stdcall = nil;
  AdrOpenSound                   : function(aDevice: TAdrAudioDevice; aSource: TAdrSampleSource; aStreaming: LongBool): TAdrOutputStream; stdcall = nil;
  AdrCreateSampleBuffer          : function(aSamples: Pointer; aFrameCount, aChannelCount, aSampleRate: Integer; aSampleFormat: TAdrSampleFormat): TAdrSampleBuffer; stdcall = nil;
  AdrCreateSampleBufferFromSource: function(aSource: TAdrSampleSource): TAdrSampleBuffer; stdcall = nil;
  AdrOpenSoundEffect             : function(aDevice: TAdrAudioDevice; aSource: TAdrSampleSource; aType: TAdrSoundEffectType): TAdrSoundEffect; stdcall = nil;

function AdrLoadDLL: Boolean; stdcall;
procedure AdrUnloadDLL; stdcall;
{$ENDIF}

implementation

{$IFDEF DYNAMIC}
uses
  Windows;

var
  AdrDLL: HMODULE = 0;

function AdrLoadDLL: Boolean;
begin
  Result := False;

  AdrDLL := LoadLibrary('audiere.dll');
  if(AdrDLL = 0) then
  begin
    Exit;
  end;

  @AdrGetVersion                   := GetProcAddress(AdrDLL, '_AdrGetVersion@0');
  @AdrGetSupportedFileFormats      := GetProcAddress(AdrDLL, '_AdrGetSupportedFileFormats@0');
  @AdrGetSupportedAudioDevices     := GetProcAddress(AdrDLL, '_AdrGetSupportedAudioDevices@0');
  @AdrGetSampleSize                := GetProcAddress(AdrDLL, '_AdrGetSampleSize@4');
  @AdrOpenDevice                   := GetProcAddress(AdrDLL, '_AdrOpenDevice@8');
  @AdrOpenSampleSource             := GetProcAddress(AdrDLL, '_AdrOpenSampleSource@8');
  @AdrOpenSampleSourceFromFile     := GetProcAddress(AdrDLL, '_AdrOpenSampleSourceFromFile@8');
  @AdrCreateTone                   := GetProcAddress(AdrDLL, '_AdrCreateTone@8');
  @AdrCreateSquareWave             := GetProcAddress(AdrDLL, '_AdrCreateSquareWave@8');
  @AdrCreateWhiteNoise             := GetProcAddress(AdrDLL, '_AdrCreateWhiteNoise@0');
  @AdrCreatePinkNoise              := GetProcAddress(AdrDLL, '_AdrCreatePinkNoise@0');
  @AdrOpenSound                    := GetProcAddress(AdrDLL, '_AdrOpenSound@12');
  @AdrCreateSampleBuffer           := GetProcAddress(AdrDLL, '_AdrCreateSampleBuffer@20');
  @AdrCreateSampleBufferFromSource := GetProcAddress(AdrDLL, '_AdrCreateSampleBufferFromSource@4');
  @AdrOpenSoundEffect              := GetProcAddress(AdrDLL, '_AdrOpenSoundEffect@12');

  if not Assigned(AdrGetVersion) then Exit;
  if not Assigned(AdrGetSupportedFileFormats) then Exit;
  if not Assigned(AdrGetSupportedAudioDevices) then Exit;
  if not Assigned(AdrGetSampleSize) then Exit;
  if not Assigned(AdrOpenDevice) then Exit;
  if not Assigned(AdrOpenSampleSource) then Exit;
  if not Assigned(AdrOpenSampleSourceFromFile) then Exit;
  if not Assigned(AdrCreateTone) then Exit;
  if not Assigned(AdrCreateSquareWave) then Exit;
  if not Assigned(AdrCreateWhiteNoise) then Exit;
  if not Assigned(AdrCreatePinkNoise) then Exit;
  if not Assigned(AdrOpenSound) then Exit;
  if not Assigned(AdrCreateSampleBuffer) then Exit;
  if not Assigned(AdrCreateSampleBufferFromSource) then Exit;
  if not Assigned(AdrOpenSoundEffect) then Exit;

  Result := True;
end;

procedure AdrUnloadDLL;
begin
  if AdrDLL <> 0 then
  begin
    FreeLibrary(AdrDLL);
    AdrDLL := 0;
  end;
end;

initialization
begin
end;

finalization
begin
  AdrUnLoadDLL;
end;

{$ENDIF}

end.
