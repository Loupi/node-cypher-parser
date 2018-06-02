# cypher-parser
A cypher graph query language parser and linter addon for node on top of libcypher-parser.

## Prerequisites
Before installing **cypher-parser** you need to assure you have the following prerequisites:

* **make, C++ and Pyton**
```sh
apk add --no-cache make gcc g++ python
```

* **libcypher-parser** see: [Installing libcypher-parser](https://github.com/cleishm/libcypher-parser/releases/tag/v0.6.0)  
If v0.6.0 is not available for your system, you can install it using:
```sh
wget https://github.com/cleishm/libcypher-parser/releases/download/v0.6.0/libcypher-parser-0.6.0.tar.gz \
&& tar zxvpf libcypher-parser-0.6.0.tar.gz \
&& rm libcypher-parser-0.6.0.tar.gz \
&& cd libcypher-parser-0.6.0 \
&& ./configure --prefix=/usr \
&& make clean check \
&& make install \
&& cd .. \
&& rm -rf libcypher-parser-0.6.0
```

## Installation
```sh
npm install --unsafe-perm cypher-parser
```

## Usage
```typescript
import * as cypher from "node-cypher-parser";
...
const query = "MATCH (node1:Label1)-->(node2:Label2)\n" +
              "WHERE node1.propertyA = {value}\n" +
              "RETURNI node2.propertyA, node2.propertyB";

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
```