
{
  "targets": [
    {
      "target_name": "binding",
      "sources": [ "src/binding.cpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      "conditions": [
          ["OS != 'h'",{
              "cflags": ["-Wall", "-Wextra", "-pedantic"],
              "cflags_cc": ["-std=c++14"],
              'cflags_cc!': [ '-fno-rtti' ]
          }]
      ]
    }
  ]
}