{=========================================================================}
{ Audiere Sound System                                                    }
{ Version 1.9.1                                                           }
{ (c) 2002 Chad Austin                                                    }
{-------------------------------------------------------------------------}
{ The original files are : audiere.h                                      }
{                                                                         }
{ The initial developer of this Pascal code was:                          }
{ Jarrod Davis                                                            }
{ JDS Games                                                               }
{ www.jdsgames.com                                                        }
{ jdsgames@jdsgames.com                                                   }
{                                                                         }
{ Audiere Sound System Delphi Adaptation                                  }
{ Portions created by JDS Games are                                       }
{ Copyright (C) 2002 JDS Games.                                           }
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
{                                                                         }
{                                                                         }
{                                                                         }
{ Revision History                                                        }
{ ----------------                                                        }
{   October  12 2002: Initial header conversion                           }
{                     Tested on D7, D6                                    }
{                                                                         }
{=========================================================================}

unit Audiere;

{$MINENUMSIZE 4}
{$ALIGN ON}

interface

type

  TSeekMode        = (smBegin, smCurr, smEnd);
  TSampleFormat    = (sfU8, sfS16);
  TSoundEffectType = (seSingle, seMultiple);

  { === TRefCounted ===================================================== }
  PRefCounted = ^TRefCounted;
  TRefCounted = object
  public
    procedure Ref;
      virtual; stdcall; abstract;
    procedure UnRef;
      virtual; stdcall; abstract;
  end;

  { === TFile =========================================================== }
  PFile = ^TFile;
  TFile = object(TRefCounted)
  public
    procedure Read(Buffer: Pointer; Size: Integer);
       virtual; stdcall; abstract;
    function  Seek(Position: Integer; Mode: TSeekMode): Boolean;
       virtual; stdcall; abstract;
    function  Tell: Integer;
       virtual; stdcall; abstract;
  end;

  { === TSampleSource =================================================== }
  PSampleSource  = ^TSampleSource;
  TSampleSource = object(TRefCounted)
  public
    procedure GetFormat(var ChannelCount: Integer; var SampleRate: Integer;
      var sample_format: TSampleFormat);
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

  { === TOutputStream =================================================== }
  POutputStream = ^TOutputStream;
  TOutputStream = object(TRefCounted)
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

  { === TAudioDevice ==================================================== }
  PAudioDevice = ^TAudioDevice;
  TAudioDevice = object(TRefCounted)
  public
    procedure Update;
      virtual; stdcall; abstract;
    function  OpenStream(Souce: PSampleSource): POutputStream;
      virtual; stdcall; abstract;
    function  OpenBuffer(Samples: Pointer; FrameCount: Integer;
      ChannelCount: Integer; SampleRate: Integer;
      SampleFormat: TSampleFormat): POutputStream;
      virtual; stdcall; abstract;
  end;

  { === TSampleBuffer =================================================== }
  PSampleBuffer = ^TSampleBuffer;
  TSampleBuffer = object(TRefCounted)
  public
    procedure GetFormat(var ChannelCount: Integer; var SampleRate: Integer;
      var SampleFormat: TSampleFormat);
      virtual; stdcall; abstract;
    function  GetLength: Integer;
      virtual; stdcall; abstract;
    function  OpneStream: PSampleSource;
      virtual; stdcall; abstract;
  end;

  { === TSoundEffect ==================================================== }
  PSoundEffect = ^TSoundEffect;
  TSoundEffect = object(TRefCounted)
  public
    procedure Play;
      virtual; stdcall; abstract;
    procedure Stop;
      virtual; stdcall; abstract;
  end;

{ === DLL Routines ====================================================== }
function AdrGetVersion: PChar;
  stdcall;
function AdrGetSupportedFileFormats: PChar;
  stdcall;
function AdrGetSampleSize(Format: TSampleFormat): Integer;
  stdcall;
function AdrOpenDevice(Name, Param: PChar): PAudioDevice;
  stdcall;
function AdrOpenSampleSource(FileName: PChar): PSampleSource;
  stdcall;
function AdrOpenSound(Device: PAudioDevice; Source: PSampleSource;
  Streaming: Boolean): POutputStream;
  stdcall;
function AdrOpenSoundFromFile(Device: PAudioDevice; FileName: PChar;
  Streaming: Boolean): POutputStream;
  stdcall;
function AdrCreateSampleBuffer(Samples: Pointer; FrameCount: Integer;
  ChannelCount: Integer; SampleRate: Integer;
  SampleFormat: TSampleFormat): PSampleBuffer;
  stdcall;
function AdrCreateSampleBufferFromSource(Source: PSampleSource)
  : TSampleBuffer;
  stdcall;
function AdrOpenSoundEffect(Device: PAudioDevice; Source: PSampleSource;
  SEType: TSoundEffectType): PSoundEffect;
  stdcall;

implementation

const
  DLLNAME = 'audiere.dll';

{ === DLL Routines ====================================================== }
function AdrGetVersion;
  external DLLNAME;
function AdrGetSupportedFileFormats;
  external DLLNAME;
function AdrGetSampleSize;
  external DLLNAME;
function AdrOpenDevice;
  external DLLNAME;
function AdrOpenSampleSource;
  external DLLNAME;
function AdrOpenSound;
  external DLLNAME;
function AdrCreateSampleBuffer;
  external DLLNAME;
function AdrCreateSampleBufferFromSource;
  external DLLNAME;
function AdrOpenSoundEffect;
  external DLLNAME;

{ === Helper Routines =================================================== }
function AdrOpenSoundFromFile(Device: PAudioDevice; FileName: PChar;
  Streaming: Boolean): POutputStream;
begin
  Result := AdrOpenSound(Device, ADrOpenSampleSource(FileName), Streaming);
end;

end.
