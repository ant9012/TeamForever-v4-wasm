#!/bin/bash
# 1. Setup libtheora
wget https://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.gz
tar -xf libtheora-1.1.1.tar.gz
mkdir -p dependencies
mv libtheora-1.1.1 dependencies/libtheora
cd dependencies/libtheora

# 2. Update config tools for modern architectures
wget -O config.guess 'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.guess;hb=HEAD'
wget -O config.sub 'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD'

# 3. Set Emscripten Flags
export CFLAGS="-s USE_OGG=1 -O3 -pthread"
export LDFLAGS="-s USE_OGG=1 -pthread"

# 4. Run configure with --host to bypass the execution check
emconfigure ./configure --host=wasm32-unknown-emscripten --disable-asm --disable-examples --disable-spec --disable-shared --enable-static --disable-encode

# 5. Build libtheora
emmake make -j$(nproc)

# 6. Return to project root for the remaining sed commands
cd ../..

# 7. Apply patches to the main engine files
# Ensure these files actually exist in these paths relative to your root!
sed -i 's/INCLUDES += $(LIBS)/# INCLUDES += $(LIBS)/' Makefile
sed -i 's/#define RETRO_PLATFORM   (RETRO_WIN)/#ifndef RETRO_PLATFORM\n#define RETRO_PLATFORM (RETRO_WIN)\n#endif/' RSDKv4/RetroEngine.hpp
sed -i 's/#include "Windows.h"//' RSDKv4/main.cpp
