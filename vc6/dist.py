import os
import types

PREFIX = os.path.abspath(ARGUMENTS.get('prefix', 'dist'))

env = Environment()
def install(target, source, files):
    if type(files) != types.ListType:
        files = [files]
    for f in files:
        env.Install(os.path.join(PREFIX, target),
                    os.path.join(source, f))

# normal C++ interface
install('bin', 'bin/Release', ['audiere.dll', 'wxPlayer.exe'])
install('doc', '../doc',
        ['changelog.txt', 'cvs.txt', 'dependencies.txt',
         'device_parameters.txt', 'faq.txt', 'glossary.txt', 'license.txt',
         'overview.txt', 'readme.txt', 'release-howto.txt',
         'small-buffers.txt', 'tutorial.txt'])
install('include',       '../src',          'audiere.h')
install('lib',           'audiere/Release', 'audiere.lib')

# Java bindings
install('bindings/java', 'bin/Release', 'javaAudiere.dll')
install('bindings/java', '../bindings/java',
        ['AudiereException.java', 'AudiereTest.java', 'AudioDevice.java',
         'OutputStream.java', 'javaAudiere.txt'])
install('bindings/java', '../bindings/java/org/aegisknight/audiere',
        ['AudiereException.class', 'AudiereTest.class', 'AudioDevice.class',
         'OutputStream.class'])

# Python bindings
install('bindings/python', 'bin/Release', 'audiere.pyd')
install('bindings/python', '../bindings/python', 'pyAudiere.txt')

# XPCOM bindings
install('bindings/xpcom', 'bin/Release', 'mozAudiere.dll')
install('bindings/xpcom', '../bindings/xpcom',
        ['mozAudiere.txt', 'adrIAudiereService.idl', 'adrIAudiereService.xpt',
         'adrIAudioDevice.idl', 'adrIAudioDevice.xpt',
         'adrIOutputStream.idl', 'adrIOutputStream.xpt'])

