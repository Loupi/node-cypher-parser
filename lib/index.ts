
const cypher = require("../build/Release/cypher");

export interface IAstNode {
  type: string;
}

export interface IParameter extends IAstNode {
  name: string;
}

export interface IMatch extends IAstNode {
  optional: boolean;
  pattern: IPattern;
  hints: IHint[];
  predicate: IPredicate;
}

export interface IQuery extends IAstNode {
  clauses: IClause[];
  options: IQueryOption[];
}

export interface IStatement extends IAstNode {
  body: IQuery|ISchemaCommand;
  options: IStatementOption[];
}

export interface ICypherOption extends IStatementOption {
  version: IString;
  params: ICypherOptionParam[];
}

export interface ICypherOptionParam extends IAstNode {
  name: IString;
  value: IString;
}

export interface IExplainOption extends IStatementOption {}
export interface IProfileOption extends IStatementOption {}

export interface ISchemaCommand extends IAstNode {}

export interface ICreateNodePropIndex extends ISchemaCommand {
  label: ILabel;
  propName: IPropName;
}

export interface IDropNodePropIndex extends ISchemaCommand {
  label: ILabel;
  propName: IPropName;
}

export interface ICreateNodePropConstraint extends ISchemaCommand {
  identifier: IIdentifier;
  label: ILabel;
  expression: IExpression;
  unique: boolean;
}

export interface IDropNodePropConstraint extends ISchemaCommand {
  identifier: IIdentifier;
  label: ILabel;
  expression: IExpression;
  unique: boolean;
}

export interface ICreateRelPropConstraint extends ISchemaCommand {
  identifier: IIdentifier;
  relType: IRelType;
  expression: IExpression;
  unique: boolean;
}

export interface IDropRelPropConstraint extends ISchemaCommand {
  identifier: IIdentifier;
  relType: IRelType;
  expression: IExpression;
  unique: boolean;
}

export interface IUsingPeriodicCommit extends IQueryOption {
  limit: number;
}

export interface IQueryClause extends IAstNode {}

export interface ILoadCsv extends IQueryClause {
  withHeaders: boolean;
  url: IExpression;
  identifier: IIdentifier;
  fieldTerminator: IString;
}

export interface IStart extends IQueryClause {
  points: IQueryOption[];
  predicate: IPredicate;
}

export interface IStartPoint extends IAstNode {}

export interface INodeIndexLookup extends IStartPoint {
  identifier: IIdentifier;
  indexName: IIndexName;
  propName: IPropName;
  lookup: IString|IParameter;
}

export interface INodeIndexQuery extends IStartPoint {
  identifier: IIdentifier;
  indexName: IIndexName;
  query: IIndexQuery;
}

export interface INodeIdLookup extends IStartPoint {
  identifier: IIdentifier;
  ids: IInteger[];
}

export interface IAllNodesScan extends IStartPoint {
  identifier: IIdentifier;
}

export interface IRelIndexQuery extends IStartPoint {
  identifier: IIdentifier;
  indexName: IIndexName;
  query: IIndexQuery;
}

export interface IRelIdLookup extends IStartPoint {
  identifier: IIdentifier;
  ids: IInteger[];
}

export interface IAllRelsScan extends IStartPoint {
  identifier: IIdentifier;
}

export interface IMatchHint extends IAstNode {}

export interface IUsingIndex extends IMatchHint {
  identifier: IIdentifier;
  label: ILabel;
  propName: IPropName;
}

export interface IUsingJoin extends IMatchHint {
  identifier: IIdentifier;
}

export interface IUsingScan extends IMatchHint {
  identifier: IIdentifier;
  label: ILabel;
}

export interface IMerge extends IClause {
  path: IPatternPath;
  actions: IMergeHint[];
}

export interface IMergeHint extends IAstNode {}

export interface IOnMatch extends IMergeHint {
  items: ISetItem[];
}

export interface IOnCreate extends IMergeHint {
  items: ISetItem[];
}

export interface ICreate extends IClause {
  unique: boolean;
  pattern: IPatternPath;
}

export interface ISet extends IClause {
  items: ISetItem[];
}

export interface ISetItem extends IAstNode {}

export interface ISetProperty extends ISetItem {
  property: IPropertyOperator;
  expression: IExpression;
}

export interface ISetAllProperties extends ISetItem {
  identifier: IIdentifier;
  expression: IExpression;
}

export interface IMergeProperties extends ISetItem {
  identifier: IIdentifier;
  expression: IExpression;
}

export interface ISetLabels extends ISetItem {
  identifier: IIdentifier;
  labels: ILabel[];
}

export interface IDelete extends IClause {
  detach: true;
  expressions: IExpression[];
}

export interface IRemove extends IClause {
  items: IExpression[];
}

export interface IRemoveItem extends IAstNode {}

export interface IRemoveLabels extends IRemoveItem {
  identifier: IIdentifier;
  labels: ILabel[];
}

export interface IRemoveProperty extends IRemoveItem {
  property: IPropertyOperator;
}

export interface IForEach extends IClause {
  identifier: IIdentifier;
  expression: IExpression;
  clauses: IClause[];
}

export interface IWith extends IClause {
  distinct: boolean;
  includeExisting: boolean;
  projections: IProjection[];
  orderBy: IOrderBy;
  skip: IInteger;
  limit: IInteger;
  predicate: IExpression;
}

export interface IUnwind extends IClause {
  expression: IExpression;
  alias: IExpression;
}

export interface ICall extends IClause {
  procName: IProcName;
  args: IExpression[];
  projections: IProjection[];
}

export interface IReturn extends IClause {
  distinct: boolean;
  includeExisting: boolean;
  projections: IProjection[];
  orderBy: IOrderBy;
  skip: IInteger;
  limit: IInteger;
}

export interface IProjection extends IAstNode {
  expression: IExpression;
  alias: IIdentifier;
}

export interface IOrderBy extends IAstNode {
  items: ISortItem[];
}

export interface ISortItem extends IAstNode {
  expression: IExpression;
  ascending: boolean;
}

export interface IUnion extends IClause {
  all: boolean;
}

export interface IUnaryOperator extends IExpression {
  op: string;
  arg: IExpression;
}

export interface IBinaryOperator extends IExpression {
  op: string;
  arg1: IExpression;
  arg2: IExpression;
}

export interface IComparison extends IExpression {
  length: number;
  ops: string[];
  args: IExpression[];
}

export interface IApplyOperator extends IExpression {
  funcName: IFunction;
  distinct: boolean;
  args: IExpression[];
}

export interface IApplyAllOperator extends IExpression {
  funcName: IFunction;
  distinct: boolean;
}

export interface IPropertyOperator extends IExpression {
  expression: IExpression;
  propName: IPropName;
}

export interface ISubscriptOperator extends IExpression {
  expression: IExpression;
  subscript: IExpression;
}

export interface ISliceOperator extends IExpression {
  expression: IExpression;
  start: IExpression;
  end: IExpression;
}

export interface IMapProjection extends IExpression {
  expression: IExpression;
  selectors: IMapProjectionSelector[];
}

export interface IMapProjectionSelector extends IAstNode {}

export interface IMapProjectionLiteral extends IMapProjectionSelector {
  propName: IPropName;
  expression: IExpression;
}

export interface IMapProjectionProperty extends IMapProjectionSelector {
  propName: IPropName;
}

export interface IMapProjectionIdentifier extends IMapProjectionSelector {
  identifier: IIdentifier;
}

export interface IMapProjectionAllProperties extends IMapProjectionSelector {}

export interface IPattern extends IAstNode {}
export interface IHint extends IAstNode {}
export interface IPredicate extends IAstNode {}
export interface IClause extends IAstNode {}
export interface IQueryOption extends IAstNode {}
export interface IStatementOption extends IAstNode {}
export interface IString extends IAstNode {}
export interface ILabel extends IAstNode {}
export interface IPropName extends IAstNode {}
export interface IIdentifier extends IAstNode {}
export interface IExpression extends IAstNode {}
export interface IRelType extends IAstNode {}
export interface IPoint extends IAstNode {}
export interface IIndexName extends IAstNode {}
export interface IIndexQuery extends IAstNode {}
export interface IInteger extends IAstNode {}
export interface IPatternPath extends IAstNode {}
export interface IProcName extends IAstNode {}
export interface IFunction extends IAstNode {}


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