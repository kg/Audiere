import os, sys

env = Environment(ENV=os.environ)

def addLib(lib):
    global env
    env.Append(LIBS="lib")

def addLibs(libs):
    for lib in libs:
        addLib(lib)

DEFINITIONS = {}

def define(name,what=True):
    global DEFINITIONS
    DEFINITIONS[name] = what
    env.Append(CFLAGS="-D" + name, CXXFLAGS="-D"+name)

def isdef(name):
    global DEFINITIONS
    return DEFINITIONS.has_key(name)

env.Append(CPPPATH=["#/src"])

if env['CC'] == 'cl':
    env.Append(CCFLAGS=['/EHsc', '/Z7', '/O2'],
               LINKFLAGS=['/DEBUG'],
               CPPDEFINES=['AUDIERE_EXPORTS', 'NOMINMAX', 'FLAC__NO_DLL'],
               CPPPATH=['#/third-party/vc6/include'],
               LIBPATH=['#/third-party/vc6/lib'],
               LIBS=['user32', 'ole32', 'rpcrt4', 'winmm'])
elif env['CC'] == 'gcc':
    env.Append(CCFLAGS=['-Wall', '-Wno-non-virtual-dtor'])

# Sanity is good
if sys.platform == 'win32':
    if ARGUMENTS.has_key('use_oss') and ARGUMENTS['use_oss'] == True:
        print "Detected Win32. Disabling OSS."
    ARGUMENTS['use_oss'] = False

if sys.platform != 'win32':
    if ARGUMENTS.has_key('use_dsound') and ARGUMENTS['use_dsound'] == True:
        print "Detected POSIX system. Disabling DirectSound."
    ARGUMENTS['use_dsound'] = False

if sys.platform not in ['win32', 'cygwin']:
    if ARGUMENTS.has_key('use_winmm') and ARGUMENTS['use_winmm'] == True:
        print "Detected POSIX system. Disabling WinMM."
    ARGUMENTS['use_winmm'] = False

conf = Configure(env)

# Parse command-line options.
if ARGUMENTS.get('use_oggvorbis', 'yes') == 'yes':
    if not conf.CheckHeader("vorbis/vorbisfile.h"):
        print "Error: Ogg Vorbis support was not found. Either install libvorbis, or rebuild with use_oggvorbis=no."
        sys.exit(1)
    if sys.platform == 'win32':
        conf.env.Append(LIBS=['vorbisfile_static', 'vorbis_static', 'ogg_static'])
    else:
        conf.env.Append(LIBS=['vorbis', 'ogg', 'vorbisfile'])
    define("HAVE_OGG")
else:
    define("NO_OGG")

if ARGUMENTS.get('use_flac', 'yes') == 'yes':
    if not conf.CheckHeader('FLAC/all.h'):
        print "Error: FLAC support was not found. Either install libFLAC, or rebuild with use_flac=no."
        sys.exit(1)
    if sys.platform == 'win32':
        conf.env.Append(LIBS=['libFLAC++_static', 'libFLAC_static'])
    else:
        conf.env.Append(LIBS=['m'])
    define("HAVE_FLAC")
else:
    define("NO_FLAC")

if ARGUMENTS.get('use_dumb', 'yes') == 'yes':
    if not conf.CheckHeader('dumb.h'):
        print "Error: libdumb was not found. Either install it, or rebuild with use_dumb=no."
        sys.exit(1)
    conf.env.Append(LIBS=['dumb_static'])
    define("HAVE_DUMB")
else:
    define("NO_DUMB")

conf.env.Append(LIBS=['libspeex'])
	
if ARGUMENTS.get('use_oss', 'yes') == 'yes':
    if not (conf.CheckHeader("unistd.h") and conf.CheckHeader("sys/soundcard.h")):
        print "Error: OSS support was not found. Either enable it, or rebuild with use_oss=no."
        sys.exit(1)
    define("HAVE_OSS")
else:
    define("NO_OSS")

if ARGUMENTS.get('use_dsound', 'yes') == 'yes':
    #conf.env.Append(LIBS=['dsound', 'ole32', 'rpcrt4'])
    if not conf.CheckHeader("dsound.h"):
        print "Error: DirectSound support was not found. Install DirectX or rebuild with use_dsound=no."
        sys.exit(1)
    define("HAVE_DSOUND")
else:
    define("NO_DSOUND")

PREFIX = ARGUMENTS.get('prefix','/usr/local')


# Handle source
source = """
	src/basic_source.cpp
	src/debug.cpp
	src/device.cpp
	src/device_mixer.cpp
	src/device_null.cpp
        src/device_mm.cpp
        src/dumb_resample.cpp
	src/file_ansi.cpp
	src/input.cpp
	src/input_aiff.cpp
	src/input_mp3.cpp
	src/input_wav.cpp
        src/input_speex.cpp
	src/loop_point_source.cpp
	src/memory_file.cpp
	src/mpaudec/bits.c
	src/mpaudec/mpaudec.c
	src/noise.cpp
	src/resampler.cpp
	src/sample_buffer.cpp
	src/sound.cpp
	src/sound_effect.cpp
	src/square_wave.cpp
	src/tone.cpp
	src/utility.cpp
	src/version.cpp
        src/speexfile/speexfile.cpp
"""

if sys.platform == 'win32':
    source += '''
        src/timer_win32.cpp
        src/threads_win32.cpp
    '''
else:
    source += '''
        src/timer_posix.cpp
        src/threads_posix.cpp
    '''

if isdef("HAVE_FLAC"):
    source += " src/input_flac.cpp"

if isdef("HAVE_DUMB"):
    source += " src/input_mod.cpp"

if isdef("HAVE_OGG"):
    source += " src/input_ogg.cpp"

if isdef("HAVE_AL"): # NOTHING CHECKS FOR THIS YET
    source += " src/device_al.cpp"

if isdef("HAVE_OSS"):
    source += " src/device_oss.cpp"

if isdef("HAVE_DSOUND"):
    source += """
        src/device_ds.cpp 
        src/device_ds_buffer.cpp
        src/device_ds_stream.cpp
        src/dxguid.cpp
    """

if isdef("HAVE_WINMM"): # NOTHING CHECKS FOR THIS YET
    source += " src/device_mm.cpp"

env = conf.Finish()

env.SharedLibrary(target='audiere', source=Split(source))

env.Program('race', 'test/race/race.cpp', LIBS=['audiere'])

#env.Install(dir = PREFIX + "/lib", source = ['libaudiere.so'])
#env.Alias('install', [PREFIX + "/lib"])

#env.Install(dir = PREFIX + "/include", source = ['src/audiere.h'])
#env.Alias('install', [PREFIX + "/include"])
