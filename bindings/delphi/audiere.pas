{=========================================================================}
{ Audiere Sound System                                                    }
{ Version 1.9.2                                                           }
{ Copyright (C) 2002 Chad Austin                                          }
{-------------------------------------------------------------------------}
{ The original files are : audiere.h                                      }
{                                                                         }
{ The initial developer of this Delphi code was:                          }
{ Jarrod Davis                                                            }
{ Jarrod Davis Software                                                   }
{ www.jarroddavis.com                                                     }
{ jarroddavis@jarroddavis.com                                             }
{                                                                         }
{ Audiere Sound System Delphi Adaptation                                  }
{ Portions created by Jarrod Davis Software                               }
{ Copyright (C) 2002-03 Jarrod Davis Software.                            }
{                                                                         }
{                                                                         }
{ Software distributed under the License is distributed on an             }
{ "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or          }
{ implied. See the License for the specific language governing            }
{ rights and limitations under the License.                               }
{                                                                         }
{ Description                                                             }
{ -----------                                                             }
{                                                                         }
{                                                                         }
{ Requires                                                                }
{ --------                                                                }
{   Audiere Runtime libraris on Win32: audiere.dll                        }
{   They are available from:                                              }
{     http://audiere.sf.net                                               }
{                                                                         }
{ Programming Notes                                                       }
{ -----------------                                                       }
{                                                                         }
{ Tested on:                                                              }
{   * Delphi 7, Update 1                                                  }
{   * Pentium II, 366Mhz                                                  }
{   * Windows 2000, SP3                                                   }
{   * Radeon 8500, latest drivers                                         }
{   * DirectX 9                                                           }
{                                                                         }
{                                                                         }
{ Revision History                                                        }
{ ----------------                                                        }
{   January  11 2003: +Synchronized with c/c++ audiere.h v1.9.2 header    }
{                     +Prefixed "Adr" to all public types                 }
{                                                                         }
{   October  12 2002: +Initial header conversion                          }
{                                                                         }
{                                                                         }
{=========================================================================}

unit Audiere;

{$MINENUMSIZE 4}
{$ALIGN ON}

interface

type

  TAdrSeekMode        = (smBegin, smCurr, smEnd);
  TAdrSampleFormat    = (sfU8, sfS16);
  TAdrSoundEffectType = (seSingle, seMultiple);

  { === TAdrRefCounted ================================================== }
  PAdrRefCounted = ^TAdrRefCounted;
  TAdrRefCounted = object
  public
    procedure Ref;
      virtual; stdcall; abstract;
    procedure UnRef;
      virtual; stdcall; abstract;
  end;

  { === TAdrFile ======================================================== }
  PAdrFile = ^TAdrFile;
  TAdrFile = object(TAdrRefCounted)
  public
    procedure Read(Buffer: Pointer; Size: Integer);
       virtual; stdcall; abstract;
    function  Seek(Position: Integer; Mode: TAdrSeekMode): Boolean;
       virtual; stdcall; abstract;
    function  Tell: Integer;
       virtual; stdcall; abstract;
  end;

  { === TAdrSampleSource ================================================ }
  PAdrSampleSource  = ^TAdrSampleSource;
  TAdrSampleSource = object(TAdrRefCounted)
  public
    procedure GetFormat(var ChannelCount: Integer; var SampleRate: Integer;
      var sample_format: TAdrSampleFormat);
      virtual; stdcall; abstract;
    function  Read(frame_count: Integer; Buffer: Pointer): Integer;
      virtual; stdcall; abstract;
    procedure Reset;
      virtual; stdcall; abstract;
    function  IsSeekable: Boolean;
      virtual; stdcall; abstract;
    function  GetLength: Integer;
      virtual; stdcall; abstract;
    procedure SetPosition(Position: Integer);
      virtual; stdcall; abstract;
    function  GetPosition: Integer;
      virtual; stdcall; abstract;
  end;

  { === TAdrOutputStream ================================================ }
  PAdrOutputStream = ^TAdrOutputStream;
  TAdrOutputStream = object(TAdrRefCounted)
  public
    procedure Play;
      virtual; stdcall; abstract;
    procedure Stop;
      virtual; stdcall; abstract;
    function  IsPlaying: Boolean;
      virtual; stdcall; abstract;
    procedure Reset;
      virtual; stdcall; abstract;
    procedure SetRepeat(Loop: Boolean);
      virtual; stdcall; abstract;
    function  GetRepeat: Boolean;
      virtual; stdcall; abstract;
    procedure SetVolume(Volume: Single);
      virtual; stdcall; abstract;
    function  GetVolume: Single;
      virtual; stdcall; abstract;
    procedure SetPan(Pan: Single);
      virtual; stdcall; abstract;
    function  GetPan: Single;
      virtual; stdcall; abstract;
    procedure SetPitchShift(Shift: Single);
      virtual; stdcall; abstract;
    function  GetPitchShift: Single;
      virtual; stdcall; abstract;
    function  IsSeekable: Boolean;
      virtual; stdcall; abstract;
    function  GetLength: Integer;
      virtual; stdcall; abstract;
    procedure SetPosition(position: Integer);
      virtual; stdcall; abstract;
    function  GetPosition: Integer;
      virtual; stdcall; abstract;
  end;

  { === TAdrAudioDevice ================================================= }
  PAdrAudioDevice = ^TAdrAudioDevice;
  TAdrAudioDevice = object(TAdrRefCounted)
  public
    procedure Update;
      virtual; stdcall; abstract;
    function  OpenStream(Souce: PAdrSampleSource): PAdrOutputStream;
      virtual; stdcall; abstract;
    function  OpenBuffer(Samples: Pointer; FrameCount: Integer;
      ChannelCount: Integer; SampleRate: Integer;
      SampleFormat: TAdrSampleFormat): PAdrOutputStream;
      virtual; stdcall; abstract;
  end;

  { === TAdrSampleBuffer ================================================ }
  PAdrSampleBuffer = ^TAdrSampleBuffer;
  TAdrSampleBuffer = object(TAdrRefCounted)
  public
    procedure GetFormat(var ChannelCount: Integer; var SampleRate: Integer;
      var SampleFormat: TAdrSampleFormat);
      virtual; stdcall; abstract;
    function  GetLength: Integer;
      virtual; stdcall; abstract;
    function GetSamples: Pointer;
      virtual; stdcall; abstract;
    function  OpenStream: PAdrSampleSource;
      virtual; stdcall; abstract;
  end;

  { === TAdrSoundEffect ================================================= }
  PAdrSoundEffect = ^TAdrSoundEffect;
  TAdrSoundEffect = object(TAdrRefCounted)
  public
    procedure Play;
      virtual; stdcall; abstract;
    procedure Stop;
      virtual; stdcall; abstract;
    procedure SetVolume(Volume: Single);
      virtual; stdcall; abstract;
    function  GetVolume: Single;
      virtual; stdcall; abstract;
    procedure SetPan(Pan: Single);
      virtual; stdcall; abstract;
    function  GetPan: Single;
      virtual; stdcall; abstract;
    procedure SetPitchShift(Shift: Single);
      virtual; stdcall; abstract;
    function  GetPitchShift: Single;
      virtual; stdcall; abstract;

  end;

{ === DLL Routines ====================================================== }
function AdrGetVersion: PChar;
  stdcall;
function AdrGetSupportedFileFormats: PChar;
  stdcall;
function AdrGetSupportedAudioDevices: PChar;
  stdcall;
function AdrGetSampleSize(Format: TAdrSampleFormat): Integer;
  stdcall;
function AdrOpenDevice(Name, Param: PChar): PAdrAudioDevice;
  stdcall;
function AdrOpenSampleSource(FileName: PChar): PAdrSampleSource;
  stdcall;
function AdrOpenSampleSourceFromFile(AFile: PAdrFile): PAdrSampleSource;
  stdcall;
function AdrOpenSoundFromFile(Device: PAdrAudioDevice; FileName: PChar;
  Streaming: Boolean): PAdrOutputStream;
  stdcall;
function AdrCreateTone(Frequency: Double): PAdrSampleSource;
  stdcall;
function AdrCreateSquareWave(Frequency: Double): PAdrSampleSource;
  stdcall;
function AdrCreateWhiteNoise: PAdrSampleSource;
  stdcall;
function AdrCreatePinkNoise: PAdrSampleSource;
  stdcall;
function AdrOpenSound(Device: PAdrAudioDevice; Source: PAdrSampleSource;
  Streaming: Boolean): PAdrOutputStream;
  stdcall;
function AdrCreateSampleBuffer(Samples: Pointer; FrameCount: Integer;
  ChannelCount: Integer; SampleRate: Integer;
  SampleFormat: TAdrSampleFormat): PAdrSampleBuffer;
  stdcall;
function AdrCreateSampleBufferFromSource(Source: PAdrSampleSource)
  : TAdrSampleBuffer;
  stdcall;
function AdrOpenSoundEffect(Device: PAdrAudioDevice; Source: PAdrSampleSource;
  SEType: TAdrSoundEffectType): PAdrSoundEffect;
  stdcall;

implementation

const
  DLLNAME = 'audiere.dll';

{ === DLL Routines ====================================================== }
function AdrGetVersion;
  external DLLNAME name '_AdrGetVersion@0';
function AdrGetSupportedFileFormats;
  external DLLNAME name '_AdrGetSupportedFileFormats@0';
function AdrGetSupportedAudioDevices;
  external DLLNAME name '_AdrGetSupportedAudioDevices@0';
function AdrGetSampleSize;
  external DLLNAME name '_AdrGetSampleSize@4';
function AdrOpenDevice;
  external DLLNAME name '_AdrOpenDevice@8';
function AdrOpenSampleSource;
  external DLLNAME name '_AdrOpenSampleSource@4';
function AdrOpenSampleSourceFromFile;
  external DLLNAME name '_AdrOpenSampleSourceFromFile@4';
function AdrCreateTone;
  external DLLNAME name '_AdrCreateTone@8';
function AdrCreateSquareWave;
  external DLLNAME name '_AdrCreateSquareWave@8';
function AdrCreateWhiteNoise;
  external DLLNAME name '_AdrCreateWhiteNoise@0';
function AdrCreatePinkNoise;
  external DLLNAME name '_AdrCreatePinkNoise@0';
function AdrOpenSound;
  external DLLNAME name '_AdrOpenSound@12';
function AdrCreateSampleBuffer;
  external DLLNAME name '_AdrCreateSampleBuffer@20';
function AdrCreateSampleBufferFromSource;
  external DLLNAME name '_AdrCreateSampleBufferFromSource@4';
function AdrOpenSoundEffect;
  external DLLNAME name '_AdrOpenSoundEffect@12';

{ === Helper Routines =================================================== }
function AdrOpenSoundFromFile(Device: PAdrAudioDevice; FileName: PChar;
  Streaming: Boolean): PAdrOutputStream;
begin
  Result := AdrOpenSound(Device, ADrOpenSampleSource(FileName), Streaming);
end;

end.
