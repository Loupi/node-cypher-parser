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
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      "conditions": [
          ["OS != 'win'", {
              "cflags": ["-Wall", "-Wextra", "-pedantic"],
              "cflags_cc": ["-std=c++14"],
              'cflags_cc!': [ '-fno-rtti' ]
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