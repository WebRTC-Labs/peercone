{
  'includes': [
    'webrtc/talk/build/common.gypi',
  ],
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'cflags': [
          '-O0',
          '-Wall',
          '-Werror',
          '-Wextra',
          '-fPIC',
          '-fcolor-diagnostics',  # TODO(pbos): Only include on clang.
          '-ftrapv',
          '-g',
          '-pedantic',
          '-std=c++11',
        ]
      },
    },
    'include_dirs': ['.']
  },
  'targets': [
    {
      'target_name': 'peercone_plugin',
      'type': 'shared_library',
      'cflags+': [ 
        '-D__PLUGIN__ -D_FILE_OFFSET_BITS=64 -D_REENTRANT -D_THREAD_SAFE -I../../../vlc/include',
        #'<!@(pkg-config --cflags vlc-plugin)',
      ],
      'sources': [
        'peercone/conductor.cc',
        'peercone/conductor.h',
        'peercone/defaults.cc',
        'peercone/defaults.h',
        'peercone/main_wnd.cc',
        'peercone/main_wnd.h',
        'peercone/peer_connection_client.cc',
        'peercone/peer_connection_client.h',
        'peercone/vlc_plugin.cc',
        'peercone/vlc_video_capturer.cc',
        'peercone/vlc_video_capturer.h',
      ],
      'libraries': [
        '-L../../../vlc/src/.libs -lvlccore',
        #'<!@(pkg-config --libs vlc-plugin)',
        # TODO(pbos): We don't intentionally depend on these.
        '<!@(pkg-config --libs-only-l glib-2.0 gobject-2.0 gthread-2.0 gtk+-2.0)',
      ],
      'include_dirs': [
        'webrtc/',
      ],
      'dependencies': [
        '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
        'webrtc/talk/libjingle.gyp:libjingle_peerconnection',
      ],
    },
    {
      'target_name': 'peercone_small',
      'type': 'executable',
      'cflags+': [
        '-D__PLUGIN__ -D_FILE_OFFSET_BITS=64 -D_REENTRANT -D_THREAD_SAFE -I../../../vlc/include',
        #'<!@(pkg-config --cflags vlc-plugin)',
      ],
      'sources': [
        'peercone/fakeaudiocapturemodule.cc',
        'peercone/peerconnectionendtoend_unittest.cc',
        'peercone/peerconnectiontestwrapper.cc',
      ],
      'libraries': [
        '-L../../../vlc/src/.libs -lvlccore',
        # TODO(pbos): We don't intentionally depend on these.
        '<!@(pkg-config --libs-only-l glib-2.0 gobject-2.0 gthread-2.0 gtk+-2.0)',
      ],
      'include_dirs': [
        'webrtc/',
      ],
      'dependencies': [
        '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
        'webrtc/talk/libjingle.gyp:libjingle_peerconnection',
      ],
    },
  ],
}
