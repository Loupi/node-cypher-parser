{
  "targets": [
    {
      "target_name": "cypher",
      "sources": [
        "addon/binding.cpp",
        "addon/parser.cpp"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")", 
        "/usr/include"
      ],
      "libraries": [
        "-lcypher-parser", "-L/usr/lib"
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
    }
  ]
}