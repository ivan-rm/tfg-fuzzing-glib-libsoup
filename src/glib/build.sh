#!/bin/bash -eu
# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

PREFIX=$WORK/prefix
mkdir -p $PREFIX

export PKG_CONFIG="$(which pkg-config) --static"
export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig
export PATH=$PREFIX/bin:$PATH

BUILD=$WORK/build

rm -rf $WORK/*
rm -rf $BUILD
mkdir -p $BUILD

# Build zlib locally so it definitely has fsanitize support
# Apply some patches from Chromium to fix known msan issues
# which are not yet fixed upstream.
pushd $SRC/zlib
patch -p1 <$SRC/0001-deflate-Zero-initialise-the-prev-and-window-buffers.patch
mkdir build && cd build
CFLAGS=-fPIC ../configure --static --prefix=$PREFIX
make install -j$(nproc)
popd

# Avoid unnecessary files (for introspection)
export FUZZ_INTROSPECTOR_CONFIG=$SRC/fuzz_introspector_exclusion.config
cat >$FUZZ_INTROSPECTOR_CONFIG <<EOF
FILES_TO_AVOID
.*/girepository/tests/.*
.*/glib/tests/.*
.*/gio/tests/.*
.*/gobject/tests/.*
.*/gmodule/tests/.*
.*/gthread/tests/.*
.*/docs/.*
.*/examples/.*
.*/subprojects/.*
.*/tests/.*
.*/tools/.*
.*/po/.*
.*/glib/fuzzing/exclude/.*
EOF

export CFLAGS="$CFLAGS -Qunused-arguments"
export CXXFLAGS="$CXXFLAGS -Qunused-arguments"

# meson $BUILD \
#     -Doss_fuzz=enabled \
#     -Db_lundef=false \
#     -Dtests=false \
#     -Dintrospection=disabled \
#     --prefix=$PREFIX \
#     --libdir=lib \
#     -Ddefault_library=static \
#     -Dlibmount=disabled

##Build GLib itself
meson $BUILD \
    --wipe \
    -Doss_fuzz=enabled \
    -Db_lundef=false \
    -Dintrospection=disabled \
    -Dtests=false \
    -Ddocumentation=false \
    -Dman-pages=disabled \
    -Dinstalled_tests=false \
    -Dglib_debug=disabled \
    -Dsysprof=disabled \
    -Dnls=disabled \
    --prefix=$PREFIX \
    --libdir=lib \
    -Ddefault_library=static \
    -Dlibmount=disabled \
    -Dselinux=disabled

ninja -C $BUILD

find $BUILD/fuzzing -maxdepth 1 -executable -type f -exec cp "{}" $OUT \;

find fuzzing -type f -name "*.dict" -exec cp "{}" $OUT \;
find fuzzing -type f -name "*.zip" -exec cp "{}" $OUT \;

#for CORPUS in $(find fuzzing -type f -name "*.corpus"); do
#  BASENAME=${CORPUS##*/}
#  zip $OUT/${BASENAME%%.*}_seed_corpus.zip . -ws -r -i@$CORPUS
#done
