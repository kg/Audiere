
{**************************************************************************
 * Audiere Sound System                                                   *
 * Version 1.9.2                                                          *
 * (c) 2002 Chad Austin                                                   *
 *                                                                        *
 * This API uses principles explained at                                  *
 * http://aegisknight.org/cppinterface.html                               *
 *                                                                        *
 * This code licensed under the terms of the LGPL.  See license.txt.      *
 *------------------------------------------------------------------------*
 * Delphi conversion by:                                                  *
 * Jarrod Davis                                                           *
 * Jarrod Davis Software                                                  *
 * http://www.jarroddavis.com                                             *
 * support@jarroddavis.com                                                *
 **************************************************************************)

{$Z4}

unit Audiere;

interface

const
  DLL_NAME = 'audiere.dll';

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
function AdrGetVersion: PChar; stdcall; external DLL_NAME name '_AdrGetVersion@0';
function AdrGetSupportedFileFormats: PChar; stdcall; external DLL_NAME name '_AdrGetSupportedFileFormats@0';
function AdrGetSupportedAudioDevices: PChar; stdcall; external DLL_NAME name '_AdrGetSupportedAudioDevices@0';
function AdrGetSampleSize(aFormat: TAdrSampleFormat): Integer; stdcall; external DLL_NAME name '_AdrGetSampleSize@4';
function AdrOpenDevice(const aName: PChar; const aParams: PChar): TAdrAudioDevice; stdcall; external DLL_NAME name '_AdrOpenDevice@8';
function AdrOpenSampleSource(const aFilename: PChar): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrOpenSampleSource@4';
function AdrOpenSampleSourceFromFile(aFile: TAdrFile): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrOpenSampleSourceFromFile@4';
function AdrCreateTone(aFrequency: Double): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreateTone@8';
function AdrCreateSquareWave(aFrequency: Double): TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreateSquareWave@8';
function AdrCreateWhiteNoise: TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreatePinkWhite@0';
function AdrCreatePinkNoise: TAdrSampleSource; stdcall; external DLL_NAME name '_AdrCreatePinkNoise@0';
function AdrOpenSound(aDevice: TAdrAudioDevice; aSource: TAdrSampleSource; aStreaming: LongBool): TAdrOutputStream; stdcall; external DLL_NAME name '_AdrOpenSound@12';
function AdrCreateSampleBuffer(aSamples: Pointer; aFrameCount, aChannelCount, aSampleRate: Integer; aSampleFormat: TAdrSampleFormat): TAdrSampleBuffer; stdcall; external DLL_NAME name '_AdrCreateSampleBuffer@20';
function AdrCreateSampleBufferFromSource(aSource: TAdrSampleSource): TAdrSampleBuffer; stdcall; external DLL_NAME name '_AdrCreateSampleBufferFromSource@4';
function AdrOpenSoundEffect(aDevice: TAdrAudioDevice; aSource: TAdrSampleSource; aType: TAdrSoundEffectType): TAdrSoundEffect; stdcall; external DLL_NAME name '_AdrOpenSoundEffect@12';

implementation

end.
