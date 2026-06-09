#!/usr/bin/env bash

set -euo pipefail

# Make sure Docker is installed.
if ! command -v docker &>/dev/null; then
  echo "Error: Docker is not installed. Aborting." >&2
  exit 1
fi
# Make sure Python 3 is installed.
if ! command -v python3 &>/dev/null; then
  echo "Error: Python 3 is not installed. Aborting." >&2
  exit 1
fi

# This script sets up the OSS-Fuzz environment in the current directory.
git clone https://github.com/google/oss-fuzz.git
pushd oss-fuzz
python3 infra/helper.py pull_images

# Clone Glib and libsoup locally.
pushd projects/glib
git clone https://gitlab.gnome.org/GNOME/glib.git
popd
pushd projects/libsoup
git clone https://gitlab.gnome.org/GNOME/libsoup.git
popd
popd

# Copy the local changes to the projects directory overwriting the existing files.
cp -r src/glib/* oss-fuzz/projects/glib/
cp -r src/libsoup/* oss-fuzz/projects/libsoup/
cp src/oss-fuzz/helper.py oss-fuzz/infra/helper.py

pushd oss-fuzz
# Build the images
python3 infra/helper.py build_image libsoup
python3 infra/helper.py build_image glib

# Build the fuzzers
python3 infra/helper.py build_fuzzers libsoup
python3 infra/helper.py build_fuzzers glib

# Copy the corpus and source code to the build directory.
mkdir -p build
popd
cp -r corpus/ oss-fuzz/build/

echo "Setup complete. Have a nice day!"
