[![Build Status](https://api.travis-ci.com/Loupi/node-cypher-parser.svg)](https://travis-ci.com/Loupi/node-cypher-parser)
![macOS](https://img.shields.io/badge/os-macOS-green.svg?style=flat)
![Linux](https://img.shields.io/badge/os-linux-green.svg?style=flat)
![TypeScript](https://badges.frapsoft.com/typescript/version/typescript-next.svg?v=101)
[![codecov](https://codecov.io/gh/Loupi/node-cypher-parser/branch/master/graph/badge.svg)](https://codecov.io/gh/Loupi/node-cypher-parser)
[![codebeat badge](https://codebeat.co/badges/f4125e60-0caf-423d-8848-4d3bb180eb33)](https://codebeat.co/projects/github-com-loupi-node-cypher-parser-master)
[![Docs](https://img.shields.io/badge/docs-typedoc-1FBCE4.svg)](https://rawgit.com/Loupi/node-cypher-parser/master/docs/index.html)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)  
[![NPM](https://nodei.co/npm/cypher-parser.png)](https://npmjs.org/package/cypher-parser)

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
- Node 8, 9, 10, 11, 12, 14
- linux-x64
- darwin-x64 (OSX >= 10.7)

Sorry Windows users, the libcypher-parser depencency cannot be built on your systems. A port is currently in progress. Meanwhile, you can still run a docker container on Windows to use it.

## Installation
```sh
npm install cypher-parser
```
The installation process will try to download a pre-built binary module matching your Node and OS version.  
If it cannot be found, you will have to first run the steps in **Custom Build**.

## Usage

The cypher-parser module has only one exported function: parse.  
It takes a query string or a ParseParameters object as input, and returns a promise as output.  
On success, the promise returns a ParseResult object or a string.  
On failure, a CypherParserError object is thrown. It contains a ParseResult object for more details.  

```typescript
export interface ParseParameters {
  query: string;      // The cypher query to parse.
  width?: number;     // Width of the text AST output. Default 0.
  dumpAst?: boolean;  // If true, the ParseResult will contain a text description of the AST tree. Default false.
  rawJson?: boolean;  // If true, the result will be a json string instead of a ParseResult object. Default false.
  colorize?: boolean; // If true, the text AST output and error descriptions will be ANSI colored. Nice for console output.
}
```  

```typescript
export interface ParseResult {
  ast: string;                        // A text description of the AST tree.
  errors: ParseError[];               // Array of parse error encountered.
  directives: parseResultDirective[]; // Parsed cypher directives.
  roots: ast.AstNode[];               // The AST tree of the parsed query. Can be walked by programs. See API doc for details.
  nnodes: number;                     // Number of nodes parsed.
}
```

* **Typescript**
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
    const result: cypher.CypherParserError = e;
    for (const error of result.parseResult.errors) {
      console.log(error.position.line + ":" + error.position.column + ": " + error.message);
      console.log(error.context);
      console.log(" ".repeat(error.contextOffset) + "^");
      console.log(result.parseResult.ast);
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
    for (var i = 0; i < e.parseResult.errors.length; i++) {
      var error = e.errors[i];
      console.log(error.position.line + ":" + error.position.column + ": " + error.message);
      console.log(error.context);
      console.log(" ".repeat(error.contextOffset) + "^");
      console.log(e.parseResult.ast);
    }
  }
}
```

## Custom Build
In case a binary distribution is not available for your system, you must install build tools and compile the libcypher-parser dependency like this:

* **make, C++ and Pyton**
```sh
sudo apk add make gcc g++ python
```

* **libcypher-parser**
```sh
wget https://github.com/cleishm/libcypher-parser/releases/download/v0.6.2/libcypher-parser-0.6.2.tar.gz \
&& tar zxvpf libcypher-parser-0.6.2.tar.gz \
&& rm libcypher-parser-0.6.2.tar.gz \
&& cd libcypher-parser-0.6.2 \
&& ./configure --prefix=/usr/local CFLAGS='-fPIC' \
&& make clean check \
&& make install \
&& cd .. \
&& rm -rf libcypher-parser-0.6.2
```
