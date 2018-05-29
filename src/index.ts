
const cypher = require("../build/Release/cypher");

export interface IPosition {
  line: number;
  column: number;
  offset: number;
}

export interface IError {
  position: IPosition;
  message: string;
  context: string;
  contextOffset: number;
}

export interface IParseResult {
  ast: string;
  errors: IError[];
}

export interface IParseParameters {
  query: string;
  width?: number;
  dumpAst?: boolean;
  colorize?: boolean;
}

export const parse = (query: string | IParseParameters) => new Promise<IParseResult>((resolve, reject) =>
  cypher.parse(query, function(succeeded: Boolean, result: IParseResult) {
    if (succeeded) {
      resolve(result);
    } else {
      reject(result);
    }
  })
);