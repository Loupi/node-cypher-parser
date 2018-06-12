export interface IAstNode {
  type: string;
}

export interface IParameter extends IAstNode {
  name: string;
}

export interface IPredicate extends IAstNode {}

export interface IMatch extends IAstNode {
  optional: boolean;
  pattern: IPattern;
  hints: IMatchHint[];
  predicate: IPredicate; // Expression instead?
}

export interface IQuery extends IAstNode {
  clauses: IClause[];
  options: IQueryOption[];
}

export interface IStatement extends IAstNode {
  body: IQuery|ISchemaCommand;
  options: IStatementOption[];
}

export interface IStatementOption extends IAstNode {}

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

export interface IQueryOption extends IAstNode {}

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
  query: IString|IParameter;
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
  query: IString|IParameter;
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

export interface IClause extends IAstNode {}

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

export interface IExpression extends IAstNode {}

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
  funcName: IFunctionName;
  distinct: boolean;
  args: IExpression[];
}

export interface IApplyAllOperator extends IExpression {
  funcName: IFunctionName;
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

export interface IListComprehension extends IExpression {
  identifier: IIdentifier;
  expression: IExpression;
  predicate: IExpression;
  eval: IExpression;
}

export interface IPatternComprehension extends IExpression {
  identifier: IIdentifier;
  pattern: IPatternPath;
  predicate: IExpression;
  eval: IExpression;
}

export interface IAlternative {
  predicate: IExpression;
  value: IExpression;
}

export interface ICase extends IExpression {
  expression: IExpression;
  alternatives: IAlternative[];
  default: IExpression;
}

export interface IFilter extends IListComprehension {
  identifier: IIdentifier;
  expression: IExpression;
  predicate: IExpression;
}

export interface IExtract extends IListComprehension {
  identifier: IIdentifier;
  expression: IExpression;
  eval: IExpression;
}

export interface IReduce extends IExpression {
  accumulator: IIdentifier;
  init: IExpression;
  identifier: IIdentifier;
  expression: IExpression;
  eval: IExpression;
}

export interface IAll extends IListComprehension {
  identifier: IIdentifier;
  expression: IExpression;
  predicate: IExpression;
}

export interface IAny extends IListComprehension {
  identifier: IIdentifier;
  expression: IExpression;
  predicate: IExpression;
}

export interface ISingle extends IListComprehension {
  identifier: IIdentifier;
  expression: IExpression;
  predicate: IExpression;
}

export interface INone extends IListComprehension {
  identifier: IIdentifier;
  expression: IExpression;
  predicate: IExpression;
}

export interface ICollection extends IExpression {
  elements: IAstNode;
}

export interface IMap extends IExpression {
  entries: { [name: string]: IAstNode };
}

export interface IIdentifier extends IExpression {
  name: string;
}

export interface IString extends IExpression {
  value: string;
}

export interface IInteger extends IExpression {
  value: number;
}

export interface IFloat extends IExpression {
  value: number;
}

export interface IBoolean extends IExpression {}
export interface ITrue extends IBoolean {}
export interface IFalse extends IBoolean {}
export interface INull extends IExpression {}

export interface ILabel extends IAstNode {
  name: string;
}

export interface IRelType extends IAstNode {
  name: string;
}

export interface IPropName extends IAstNode {
  value: string;
}

export interface IFunctionName extends IAstNode {
  value: string;
}

export interface IIndexName extends IAstNode {
  value: string;
}

export interface IProcName extends IAstNode {
  value: string;
}

export interface IPattern extends IAstNode {
  paths: IPatternPath[];
}

export type patternPathElement = INodePattern | IRelPattern;

export interface IPatternPath extends IAstNode {
  elements: patternPathElement[];
}

export interface INamedPath extends IPatternPath {
  identifier: IIdentifier;
  path: IPatternPath;
}

export interface IShortestPath extends IPatternPath {
  single: boolean;
  path: IPatternPath;
}

export interface INodePattern extends IAstNode {
  identifier: IIdentifier;
  labels: ILabel[];
  properties: IMap|IParameter;
}

export interface IRelPattern extends IAstNode {
  direction: number;
  identifier: IIdentifier;
  relTypes: ILabel[];
  properties: IMap|IParameter;
  varLength: IRange;
}

export interface IRange extends IAstNode {
  start: number;
  end: number;
}

export interface ICommand extends IAstNode {
  name: string;
  args: IString[];
}

export interface IComment extends IAstNode {
  value: string;
}

export interface ILineComment extends IComment {}

export interface IBlockComment extends IComment {}

export interface IError extends IAstNode {
  value: string;
}