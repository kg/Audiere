#!/bin/sh

die() {
    echo "Error, aborting..."
    exit 1
}

echo
echo "Building VC6 workspace..."

msdev vc6/audiere.dsw /MAKE ALL /OUT vc6-build.log || die

echo
echo "Build log written to vc6-build.log"
echo

NAME=audiere-1.9.3-win32
PREFIX=../dist/$NAME
scons -C vc6 -f dist.py prefix=$PREFIX $PREFIX || die
find dist -name .sconsign | xargs rm -f
rm -f dist/$NAME.zip
(cd dist && zip -r $NAME.zip $NAME) || die

echo
echo "Win32 release completed"
