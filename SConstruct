# SCons build script for MIPSPro

import os

# build the entire source tree by default
Default('.')

# get installation prefix
PREFIX = ARGUMENTS.get('prefix', '/usr/local')

CXXFLAGS = ['-DHAVE_AL',
            '-DNO_MIKMOD',
            '-DNO_FLAC',
            '-DNO_MP3',
            '-DWORDS_BIGENDIAN']

# build debug?
if ARGUMENTS.get('debug', 0):
    CXXFLAGS.extend(['-g', '-DDEBUG'])

base_env = Environment(
    CPPPATH = ['/usr/freeware/include'],
    LIBPATH = ['/usr/freeware/lib32'],
    LIBS = ['pthread', 'vorbisfile', 'vorbis', 'ogg', 'audio', 'm'])
base_env.Append(CXXFLAGS = CXXFLAGS)

Export('PREFIX base_env')

SConscript(dirs = ['src', 'examples', 'test'])

base_env.Alias('install', PREFIX)
