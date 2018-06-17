import "mocha";
import { expect } from "chai";
import * as cypher from "../src/index";

const query = "MATCH (node1:Label1)-->(node2:Label2)\n" +
              "WHERE node1.propertyA = {value}\n" +
              "RETURN node2.propertyA, node2.propertyB";

const badQuery = "MATCH (node1:Label1)-->(node2:Label2)\n" +
              "WHERE node1.propertyA = {value}\n" +
              "RETURNI node2.propertyA, node2.propertyB";

describe("cypher.parse", () => {

  describe("given object parameters", () => {
    it("should return a valid parse result", async () => {
      const promise = cypher.parse({query, dumpAst: true});
      const result = await promise;
      expect(promise).to.be.a("promise");
      expect(result).to.be.an("object");
      expect(result).to.have.property("ast");
      expect(result.ast).to.be.a("string");
      expect(result).to.have.property("nnodes");
      expect(result.nnodes).to.be.a("number");
      expect(result).to.have.property("roots");
      expect(result.roots).to.be.an("array");
    });
  });

  describe("given string parameter", () => {
    it("should return a valid parse result", async () => {
      const promise = cypher.parse(query);
      const result = await promise;
      expect(promise).to.be.a("promise");
      expect(result).to.be.an("object");
      expect(result).not.to.have.property("ast");
      expect(result).to.have.property("nnodes");
      expect(result.nnodes).to.be.a("number");
      expect(result).to.have.property("roots");
      expect(result.roots).to.be.an("array");
    });
  });

  describe("given rawJson option", () => {
    it("should return a json string", async () => {
      const promise = cypher.parse({query, rawJson: true});
      const result = await promise;
      expect(promise).to.be.a("promise");
      expect(result).to.be.a("string");
    });
  });

  describe("given bad query", () => {
    it("should throw parse error", async () => {
      try {
        await cypher.parse(badQuery);
        expect.fail();
      }
      catch (error) {
        expect(error).to.have.property("parseResult");
        expect(error.parseResult).to.be.an("object");
        expect(error.parseResult).to.have.property("errors");
        expect(error.parseResult.errors).to.be.an("array");
      }
    });
  });

  describe("given bad query and rawJson option", () => {
    it("should throw parse error as json string", async () => {
      try {
        await cypher.parse({query: badQuery, rawJson: true});
        expect.fail();
      }
      catch (error) {
        expect(error).to.have.property("parseResult");
        expect(error.parseResult).to.be.a("string");
      }
    });
  });
});