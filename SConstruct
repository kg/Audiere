# SCons build script for MIPSPro

import os

# build the entire source tree by default
Default('.')

# get installation prefix
PREFIX = ARGUMENTS.get('prefix', '/usr/local')

CXXFLAGS = ['-LANG:std',
            '-DHAVE_AL',
            '-DNO_MIKMOD',
            '-DNO_FLAC',
            '-DWORDS_BIGENDIAN']

# build debug?
if ARGUMENTS.get('debug', 0):
    CXXFLAGS.extend(['-g', '-DDEBUG'])

base_env = Environment(
    CXX = 'CC',
    CXXFLAGS = CXXFLAGS,
    LINK = 'CC',
    LINKFLAGS = ['-LANG:std'],
    SHCXXFLAGS = CXXFLAGS,
    SHLINK = 'CC',
    SHLINKFLAGS = ['-shared'],
    CPPPATH = ['/usr/freeware/include'],
    LIBPATH = ['/usr/freeware/lib32'],
    LIBS = ['vorbisfile', 'vorbis', 'ogg', 'audio', 'm'])

Export('PREFIX base_env')

SConscript(dirs = ['src', 'examples', 'test'])

base_env.Alias('install', PREFIX)
