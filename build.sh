#!/bin/bash
set -x
webrtc/build/gyp_chromium --depth=webrtc/ peercone.gyp
ninja -C webrtc/out/Debug/
