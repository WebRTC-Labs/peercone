#!/bin/bash
set -x

# If VLC configure usually has troubles with missing dependencies,
# the following line should solve them out.
# sudo aptitude install libvpx-dev libavcodec-dev libavformat-dev libswscale-dev libvorbis-dev libogg-dev libx11-xcb-dev libxcb-composite0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-shm0-dev libxcb-xv0-dev libxcb-xvmc0-dev libxcb1-dev libxext-dev libxml2-dev libxpm-dev libxt-dev libxv-dev

echo "Compiling VLC ---------------------------"
cd vlc && \
./bootstrap && \
./configure --disable-live555 --disable-linsys --disable-mad --enable-vpx --disable-a52 --disable-png --disable-jpeg --disable-x264 --disable-zvbi --disable-libass --disable-qt --disable-projectm --disable-atmo --disable-libgcrypt --disable-lua  && \
make

echo "Compiling webrtc peercon ----------------"
webrtc/build/gyp_chromium --depth=webrtc/ peercone.gyp
ninja -C webrtc/out/Debug/
