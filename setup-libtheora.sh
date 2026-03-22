#!/bin/bash
wget https://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.gz
tar -xf libtheora-1.1.1.tar.gz
mv libtheora-1.1.1 dependencies/libtheora
cd dependencies/libtheora
wget -O config.guess 'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.guess;hb=HEAD'
wget -O config.sub 'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD'

          # CFLAGS must include -pthread so it links with the engine correctly
export CFLAGS="-s USE_OGG=1 -O3 -pthread"
export LDFLAGS="-s USE_OGG=1 -pthread"
emconfigure ./configure --disable-asm --disable-examples --disable-spec --disable-shared --enable-static --disable-encode
emmake make -j$(nproc)
