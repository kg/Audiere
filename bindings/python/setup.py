#!/usr/bin/env python

from distutils.core import setup, Extension

ext_src = ['AudioDevice.cpp', 'pyAudiere.cpp', 'OutputStream.cpp']
ext = Extension('audiere', ext_src, libraries = ['audiere', 'stdc++'])

setup(name = 'audiere',
      version = '1.9.2',
      description = 'A high-level audio API.',
      url = 'http://audiere.sourceforge.net/',
      ext_modules = [ ext ])
