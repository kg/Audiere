#!/bin/sh

die() {
    echo "Error, aborting..."
    exit 1
}

vc6die() {
    echo "Error building audiere.dsw"
    echo "See build log: vc6-build.log"
    exit 1
}

echo
echo "Building Java bindings..."
echo

cd bindings/java || die
make || die
cd ../.. || die

echo
echo "Building XPCOM bindings..."
echo

cd bindings/xpcom || die
scons || die
cd ../.. || die

echo
echo "Building Audiere VC6 workspace..."

msdev vc6/audiere.dsw /MAKE ALL /OUT vc6-build.log || vc6die

echo
echo "Build complete"
echo

NAME=audiere-1.9.3-win32
PREFIX=../dist/$NAME
scons -C vc6 -f dist.py prefix=$PREFIX $PREFIX || die
find dist -name .sconsign | xargs rm -f
rm -f dist/$NAME.zip
(cd dist && zip -r $NAME.zip $NAME) || die

echo
echo "Win32 release completed"
