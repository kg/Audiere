#!/bin/sh

die() {
    echo "Error, aborting..."
    exit 1
}

VERSION=1.9.3
NAME=audiere-$VERSION-sgi
FILE=$NAME.tar.bz2

scons prefix=dist/$NAME dist/$NAME || die
cd dist || die
/usr/freeware/bin/tar cfj $FILE $NAME || die

echo
echo "SGI release built: dist/${FILE}"
