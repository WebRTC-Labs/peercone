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
        'peercone/peer_connection_client.cc',
        'peercone/peer_connection_client.h',
      ],
      'include_dirs+': [
        'webrtc/',
      ],
      'dependencies': [
        '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
        'webrtc/talk/libjingle.gyp:libjingle_peerconnection',
      ],
      'conditions': [
        ['OS=="linux"', {
          'sources': [
            'peercone/linux/main.cc',
            'peercone/linux/main_wnd.cc',
            'peercone/linux/main_wnd.h',
          ],
          'cflags': [
            '<!@(pkg-config --cflags glib-2.0 gobject-2.0 gtk+-2.0)',
          ],
          'link_settings': {
            'ldflags': [
              '<!@(pkg-config --libs-only-L --libs-only-other glib-2.0'
               ' gobject-2.0 gthread-2.0 gtk+-2.0)',
            ],
            'libraries': [
              '<!@(pkg-config --libs-only-l glib-2.0 gobject-2.0'
                ' gthread-2.0 gtk+-2.0)',
              '-lX11',
              '-lXcomposite',
              '-lXext',
              '-lXrender',
            ],
          },
        }],  # OS=="linux"
      ],  # conditions
    },  # target peerconnection_client
  ],
}
