# SCons build script for SGI MIPSPro

import os

# get installation prefix
PREFIX = ARGUMENTS.get('prefix', '/usr/local')

CXXFLAGS = ['-DHAVE_AL',
            '-DNO_DUMB',
            '-DWORDS_BIGENDIAN']

# build debug?
if ARGUMENTS.get('debug', 0):
    CXXFLAGS.extend(['-g', '-DDEBUG'])
else:
    CXXFLAGS.extend(['-O2'])

base_env = Environment(
    CPPPATH = ['/usr/freeware/include'],
    LIBPATH = ['/usr/freeware/lib32'],
    LIBS = ['pthread', 'vorbisfile', 'vorbis', 'ogg', 'FLAC++', 'FLAC', 'audio', 'm'])
base_env.Append(CXXFLAGS = CXXFLAGS)

Export('PREFIX base_env')

SConscript(dirs = ['src', 'examples', 'test'])

base_env.Alias('install', PREFIX)
