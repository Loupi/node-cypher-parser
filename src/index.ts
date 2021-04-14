const binary = require("@mapbox/node-pre-gyp");
const path = require("path");
const binding_path = binary.find(path.resolve(path.join(__dirname, "../package.json")));
const cypher = require(binding_path);
import * as ast from "./ast";

export interface ParsePosition {
  line: number;
  column: number;
  offset: number;
}

export interface ParseError {
  position: ParsePosition;
  message: string;
  context: string;
  contextOffset: number;
}

export type parseResultDirective = ast.Statement|ast.Command;
export interface ParseResult {
  ast: string;
  errors: ParseError[];
  directives: parseResultDirective[];
  roots: ast.AstNode[];
  nnodes: number;
}

export interface ParseParameters {
  query: string;
  width?: number;
  dumpAst?: boolean;
  rawJson?: boolean;
  colorize?: boolean;
  parseOnlyStatements?: boolean;
}

export class CypherParserError extends Error {
  constructor(parseResult: ParseResult) {
      super("Cypher Parser Error");
      this.parseResult = parseResult;
      Object.setPrototypeOf(this, CypherParserError.prototype);
  }

  parseResult: ParseResult;
}

export const parse = (query: string | ParseParameters) => new Promise<ParseResult>((resolve, reject) =>
  cypher.parse(function(succeeded: boolean, result: ParseResult) {
    if (succeeded) {
      resolve(result);
    } else {
      reject(new CypherParserError(result));
    }
  }, query)
);