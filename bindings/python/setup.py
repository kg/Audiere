#!python

from distutils.core import setup, Extension

ext_src = [ 'context.cpp', 'pyAudio.cpp', 'stream.cpp' ]
ext = Extension('audiere', ext_src, libraries = ['audiere', 'stdc++'])

setup(name = 'audiere',
      version = '1.0',
      ext_modules = [ ext ])
