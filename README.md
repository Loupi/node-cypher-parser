[![Docs](https://img.shields.io/badge/docs-cypher--parser-1FBCE4.svg)](https://rawgit.com/Loupi/node-cypher-parser/master/docs/index.html)

[![Build Status](https://api.travis-ci.com/Loupi/node-cypher-parser.svg)](https://travis-ci.com/Loupi/node-cypher-parser)

![macOS](https://img.shields.io/badge/os-macOS-green.svg?style=flat)

![Linux](https://img.shields.io/badge/os-linux-green.svg?style=flat)

[![codecov](https://codecov.io/gh/Loupi/node-cypher-parser/branch/master/graph/badge.svg)](https://codecov.io/gh/Loupi/node-cypher-parser)

[![codebeat](https://codebeat.co/badges/b9441fbf-77dc-4159-8cf5-0442ba7ac3ba)](https://codebeat.co/projects/github-com-loupi-node-cypher-parser-master)

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

# cypher-parser
A cypher graph query language parser/linter addon module for NodeJS.  
It relies on [libcypher-parser](https://github.com/cleishm/libcypher-parser) and [rapidjson](https://github.com/Tencent/rapidjson) under the hood.

## Features

* Promise support
* C++ addon parses queries in a worker thread
* Typescript support with interfaces defined for full AST
* Outputs json AST
* Outputs text description of AST
* Outputs error with position and description
* Optional ANSI color support for text and error output
* [API Documentation](https://rawgit.com/Loupi/node-cypher-parser/master/docs/index.html)

## Supported Systems
- Node 6.x, 8.x, 9.x, 10.x
- linux-x64
- darwin-x64 (OSX >= 10.7)

Sorry Windows users, the libcypher-parser depencency cannot be built on your systems.  You can still run a docker container on Windows to use it.

## Installation
```sh
npm install cypher-parser
```
The installation process will try to download a pre-built binary module matching your Node and OS version.  
If it cannot be found, you will have to first run the steps in **Custom Build**.

## Custom Build
In case a binary distribution is not available for your system, you must install build tools and compile the libcypher-parser dependency like this:

* **make, C++ and Pyton**
```sh
sudo apk add make gcc g++ python
```

* **libcypher-parser**
```sh
wget https://github.com/cleishm/libcypher-parser/releases/download/v0.6.0/libcypher-parser-0.6.0.tar.gz \
&& tar zxvpf libcypher-parser-0.6.0.tar.gz \
&& rm libcypher-parser-0.6.0.tar.gz \
&& cd libcypher-parser-0.6.0 \
&& ./configure --prefix=/usr/local CFLAGS='-fPIC' \
&& make clean check \
&& make install \
&& cd .. \
&& rm -rf libcypher-parser-0.6.0
```

## Usage
* **Typscript**
```typescript
import * as cypher from "cypher-parser";

async function testCypher() {
  const query = "MATCH (node1:Label1)-->(node2:Label2)\n" +
    "WHERE node1.propertyA = {value}\n" +
    "RETURN node2.propertyA, node2.propertyB";

  try {
    const result = await cypher.parse({
      query: query,
      dumpAst: true,
      colorize: true
    });
    console.log(result.ast);
  } catch (e) {
    const result: cypher.IParseResult = e;
    for (const error of result.errors) {
      console.log(error.position.line + ":" + error.position.column + ": " + error.message);
      console.log(error.context);
      console.log(" ".repeat(error.contextOffset) + "^");
      console.log(result.ast);
    }
  }
}
```

* **Javascript**
```Javascript
var cypher = require('cypher-parser');

async function testCypher() {
  var query = "MATCH (node1:Label1)-->(node2:Label2)\n" +
    "WHERE node1.propertyA = {value}\n" +
    "RETURN node2.propertyA, node2.propertyB";

  try {
    var result = await cypher.parse({
      query: query,
      dumpAst: true,
      colorize: true
    });
    console.log(result.ast);
  } catch (e) {
    for (var i = 0; i < e.errors.length; i++) {
      var error = e.errors[i];
      console.log(error.position.line + ":" + error.position.column + ": " + error.message);
      console.log(error.context);
      console.log(" ".repeat(error.contextOffset) + "^");
      console.log(e.ast);
    }
  }
}
```
