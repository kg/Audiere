#!/usr/bin/env python

from distutils.core import setup, Extension

ext_src = ['AudioDevice.cpp', 'pyAudiere.cpp', 'OutputStream.cpp']
ext = Extension('audiere', ext_src, libraries = ['audiere', 'stdc++'])

setup(name = 'audiere',
      version = '1.9.1',
      description = 'A high-level audio API.',
      url = 'http://audiere.sf.net/',
      ext_modules = [ ext ])
