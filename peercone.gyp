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
        '<!@(pkg-config --cflags vlc-plugin)',
      ],
      'sources': [
        'peercone/vlc-plugin.cc',
      ],
      'libraries': [
        '<!@(pkg-config --libs vlc-plugin)',
      ],
    },
    {
      'target_name': 'peercone_cli',
      'type': 'executable',
      'sources': [
        'peercone/conductor.cc',
        'peercone/conductor.h',
        'peercone/defaults.cc',
        'peercone/defaults.h',
        'peercone/main.cc',
        'peercone/main_wnd.cc',
        'peercone/main_wnd.h',
        'peercone/peer_connection_client.cc',
        'peercone/peer_connection_client.h',
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
