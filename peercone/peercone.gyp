{
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines': ['DEBUG'],
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
    'include_dirs': ['..']
  },
  'targets': [
    {
      'target_name': 'peercone_plugin',
      'type': 'shared_library',
      'cflags+': [
        '<!@(pkg-config --cflags vlc-plugin)',
      ],
      'sources': [
        'vlc-plugin.cc',
      ],
      'libraries': [
        '<!@(pkg-config --libs vlc-plugin)',
      ],
    },
  ],
}
