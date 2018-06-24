import * as cypher from "cypher-parser";

async function parseCypher() {
  const query = "MATCH (node1:Label1)-->(node2:Label2)\n" +
                "WHERE node1.propertyA = {value}\n" +
                "RETURNI node2.propertyA, node2.propertyB";

  console.time("cypher-parser");
  try {
    const result = await cypher.parse({
      query: query,
      dumpAst: true,
      colorize: true
    });
    console.timeEnd("cypher-parser");
    console.log(result.ast);
  } catch (e) {
    console.timeEnd("cypher-parser");
    const result: cypher.CypherParserError = e;
    for (const error of result.parseResult.errors) {
      console.log(error.position.line + ":" + error.position.column + ": " + error.message);
      console.log(error.context);
      console.log(" ".repeat(error.contextOffset) + "^");
      console.log(result.parseResult.ast);
    }
  }
}

parseCypher();