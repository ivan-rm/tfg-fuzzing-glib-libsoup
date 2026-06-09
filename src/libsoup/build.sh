# Force the linker to see the missing static symbols
# Required because linking against a static GLib 2.82

# Avoid unnecessary file (for introspection)
export FUZZ_INTROSPECTOR_CONFIG=$SRC/fuzz_introspector_exclusion.config
cat >$FUZZ_INTROSPECTOR_CONFIG <<EOF
FILES_TO_AVOID
tests
subprojects
examples
docs
po
EOF

#Patch the fuzzing check
#sed -i 's/have_fuzzing = cc.has_argument(fuzzing_args)/have_fuzzing = true/g' "$SRC/libsoup/fuzzing/meson.build"
# The :- syntax provides an empty default if LDFLAGS is unbound/unset

# -Db_sanitize=none because $CFLAGS already have the sanitizer flags.
meson setup "$WORK" --wipe --buildtype=debug \
    -Db_lundef=false -Dtls_check=false -Dsysprof=disabled \
    -Dfuzzing=enabled -Dprefer_static=true -Ddefault_library=static \
    -Dintrospection=disabled \
    -Db_sanitize=none \
    -Dtests=false \
    -Dintrospection=disabled \
    -Dvapi=disabled \
    -Ddocs=disabled \
    -Ddoc_tests=false \
    -Dautobahn=disabled \
    -Dinstalled_tests=false \
    -Dpkcs11_tests=disabled

meson compile -C "$WORK"

find "$WORK/fuzzing" -perm /a+x -type f -exec cp {} "$OUT" \;
find fuzzing -type f -name "*.dict" -exec cp "{}" $OUT \;
find fuzzing -type f -name "*.zip" -exec cp "{}" $OUT \;
