{
  "targets": [
    {
      "target_name": "cypher",
      "sources": [
        "addon/binding.cpp",
        "addon/parser.cpp",
        "addon/memstream/memstream.c"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")", 
        "/usrlocal/include"
      ],
      "libraries": [
        "/usr/local/lib/libcypher-parser.a", "-L/usr/lib"
      ],
      "cflags!": [ "-fno-exceptions" ],
      'cflags_cc!': [ '-fno-exceptions' ],
      "conditions": [
        ["OS=='linux' or OS=='freebsd' or OS=='openbsd' or OS=='solaris'", {
            "cflags": ["-Wall", "-Wextra", "-pedantic"],
            "cflags_cc": ["-std=c++14"],
            'cflags_cc!': [ '-fno-rtti' ]
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'ALWAYS_SEARCH_USER_PATHS': 'NO',
            'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
            'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                      # (Equivalent to -fPIC)
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',       # -fno-exceptions
            'GCC_ENABLE_CPP_RTTI': 'NO',              # -fno-rtti
            'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
            'GCC_THREADSAFE_STATICS': 'NO',           # -fno-threadsafe-statics
            'GCC_VERSION': '6',
            'GCC_WARN_ABOUT_MISSING_NEWLINE': 'NO',  # -Wnewline-eof
            'PREBINDING': 'NO',                       # No -Wl,-prebind
            'USE_HEADERMAP': 'NO',
            'OTHER_CFLAGS': [
              '-fno-strict-aliasing',
            ],
            'WARNING_CFLAGS': [
              '-Wall',
              '-Wendif-labels',
              '-W',
              '-Wno-unused-parameter'
            ],
          }
        }]
      ]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}