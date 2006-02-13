#!/usr/bin/env python

import os
from distutils.core import setup, Extension

ext_src = ['AudioDevice.cpp', 'pyAudiere.cpp', 'OutputStream.cpp']
libs = ['audiere']
if os.name == 'posix':
    libs.append('stdc++')
ext = Extension('audiere', ext_src,
                include_dirs=['../../src'],
                library_dirs=['../../src'],
                libraries = libs)

setup(name = 'audiere',
      version = '1.9.4',
      description = 'A high-level audio API.',
      url = 'http://audiere.sourceforge.net/',
      ext_modules = [ ext ])
