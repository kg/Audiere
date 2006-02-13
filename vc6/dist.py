PREFIX = ARGUMENTS.get('prefix', 'dist')

env = Environment(tools=[])
def install(target, source, files):
    from os.path import join
    from types import ListType

    if type(files) != ListType:
        files = [files]
    for f in files:
        env.Install(join(PREFIX, target), join(source, f))

# normal C++ interface
install('bin', 'vc6/bin/Release', ['audiere.dll', 'wxPlayer.exe'])
install('doc', 'doc',
        ['authors.txt', 'changelog.txt', 'cvs.txt', 'dependencies.txt',
         'device_parameters.txt', 'faq.txt', 'glossary.txt', 'license.txt',
         'overview.txt', 'readme.txt', 'release-howto.txt',
         'small-buffers.txt', 'tutorial.txt'])
install('include', 'src', 'audiere.h')
install('lib', 'vc6/audiere/Release', 'audiere.lib')

# Java bindings
install('bindings/java', 'vc6/bin/Release', 'javaAudiere.dll')
install('bindings/java', 'bindings/java',
        ['AudiereException.java', 'AudiereTest.java', 'AudioDevice.java',
         'OutputStream.java', 'javaAudiere.txt'])
install('bindings/java', 'bindings/java/org/aegisknight/audiere',
        ['AudiereException.class', 'AudiereTest.class', 'AudioDevice.class',
         'OutputStream.class'])

# Python bindings
install('bindings/python', 'vc6/bin/Release', 'audiere.pyd')
install('bindings/python', 'bindings/python', 'pyAudiere.txt')

# XPCOM bindings
install('bindings/xpcom', 'vc6/bin/Release', 'mozAudiere.dll')
install('bindings/xpcom', 'bindings/xpcom',
        ['mozAudiere.txt', 'adrIAudiereService.idl', 'adrIAudiereService.xpt',
         'adrIAudioDevice.idl', 'adrIAudioDevice.xpt',
         'adrIOutputStream.idl', 'adrIOutputStream.xpt'])
