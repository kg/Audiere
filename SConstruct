# SCons build script for MIPSPro

import os

# build the entire source tree by default
Default('.')

# get installation prefix
PREFIX = ARGUMENTS.get('prefix', '/usr/local')

base_env = Environment(
    CXX = 'CC',
    CXXFLAGS = ['-LANG:std'],
    LINK = 'CC',
    LINKFLAGS = ['-LANG:std'],
    SHCXXFLAGS = ['-LANG:std'],
    SHLINK = 'CC',
    SHLINKFLAGS = ['-shared'],
    CPPPATH = ['/usr/freeware/include'],
    LIBPATH = ['/usr/freeware/lib32'],
    LIBS = ['vorbisfile', 'vorbis', 'ogg'])

Export('PREFIX base_env')

SConscript(dirs = ['src', 'test'])

base_env.Alias('install', PREFIX)
