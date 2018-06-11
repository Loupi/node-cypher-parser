import "mocha";
import { expect } from "chai";
import * as cypher from "../lib/index";

const query = "MATCH (node1:Label1)-->(node2:Label2)\n" +
              "WHERE node1.propertyA = {value}\n" +
              "RETURN node2.propertyA, node2.propertyB";

describe("parse query", () => {
  it("should return a valid parse result", async () => {
    const result = await cypher.parse({query, dumpAst: true});
    expect(result).to.be.an("object");
    expect(result).to.have.property("ast");
    expect(result.ast).to.be.a("string");
    expect(result).to.have.property("nnodes");
    expect(result.nnodes).to.be.a("number");
    expect(result).to.have.property("roots");
    expect(result.roots).to.be.an("array");
  });
});