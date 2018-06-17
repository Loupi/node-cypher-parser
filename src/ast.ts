export interface AstNode {
  type: string;
}

export interface Parameter extends AstNode {
  name: string;
}

export interface Predicate extends AstNode {}

export interface Match extends AstNode {
  optional: boolean;
  pattern: Pattern;
  hints: MatchHint[];
  predicate: Predicate; // Expression instead?
}

export interface Query extends AstNode {
  clauses: Clause[];
  options: QueryOption[];
}

export interface Statement extends AstNode {
  body: Query|SchemaCommand;
  options: StatementOption[];
}

export interface StatementOption extends AstNode {}

export interface CypherOption extends StatementOption {
  version: String;
  params: CypherOptionParam[];
}

export interface CypherOptionParam extends AstNode {
  name: String;
  value: String;
}

export interface ExplainOption extends StatementOption {}
export interface ProfileOption extends StatementOption {}

export interface SchemaCommand extends AstNode {}

export interface CreateNodePropIndex extends SchemaCommand {
  label: Label;
  propName: PropName;
}

export interface DropNodePropIndex extends SchemaCommand {
  label: Label;
  propName: PropName;
}

export interface CreateNodePropConstraint extends SchemaCommand {
  identifier: Identifier;
  label: Label;
  expression: Expression;
  unique: boolean;
}

export interface DropNodePropConstraint extends SchemaCommand {
  identifier: Identifier;
  label: Label;
  expression: Expression;
  unique: boolean;
}

export interface CreateRelPropConstraint extends SchemaCommand {
  identifier: Identifier;
  relType: RelType;
  expression: Expression;
  unique: boolean;
}

export interface DropRelPropConstraint extends SchemaCommand {
  identifier: Identifier;
  relType: RelType;
  expression: Expression;
  unique: boolean;
}

export interface QueryOption extends AstNode {}

export interface UsingPeriodicCommit extends QueryOption {
  limit: number;
}

export interface QueryClause extends AstNode {}

export interface LoadCsv extends QueryClause {
  withHeaders: boolean;
  url: Expression;
  identifier: Identifier;
  fieldTerminator: String;
}

export interface Start extends QueryClause {
  points: QueryOption[];
  predicate: Predicate;
}

export interface StartPoint extends AstNode {}

export interface NodeIndexLookup extends StartPoint {
  identifier: Identifier;
  indexName: IndexName;
  propName: PropName;
  lookup: String|Parameter;
}

export interface NodeIndexQuery extends StartPoint {
  identifier: Identifier;
  indexName: IndexName;
  query: String|Parameter;
}

export interface NodeIdLookup extends StartPoint {
  identifier: Identifier;
  ids: Integer[];
}

export interface AllNodesScan extends StartPoint {
  identifier: Identifier;
}

export interface RelIndexQuery extends StartPoint {
  identifier: Identifier;
  indexName: IndexName;
  query: String|Parameter;
}

export interface RelIdLookup extends StartPoint {
  identifier: Identifier;
  ids: Integer[];
}

export interface AllRelsScan extends StartPoint {
  identifier: Identifier;
}

export interface MatchHint extends AstNode {}

export interface UsingIndex extends MatchHint {
  identifier: Identifier;
  label: Label;
  propName: PropName;
}

export interface UsingJoin extends MatchHint {
  identifier: Identifier;
}

export interface UsingScan extends MatchHint {
  identifier: Identifier;
  label: Label;
}

export interface Clause extends AstNode {}

export interface Merge extends Clause {
  path: PatternPath;
  actions: MergeHint[];
}

export interface MergeHint extends AstNode {}

export interface OnMatch extends MergeHint {
  items: SetItem[];
}

export interface OnCreate extends MergeHint {
  items: SetItem[];
}

export interface Create extends Clause {
  unique: boolean;
  pattern: PatternPath;
}

export interface Set extends Clause {
  items: SetItem[];
}

export interface SetItem extends AstNode {}

export interface SetProperty extends SetItem {
  property: PropertyOperator;
  expression: Expression;
}

export interface SetAllProperties extends SetItem {
  identifier: Identifier;
  expression: Expression;
}

export interface MergeProperties extends SetItem {
  identifier: Identifier;
  expression: Expression;
}

export interface SetLabels extends SetItem {
  identifier: Identifier;
  labels: Label[];
}

export interface Delete extends Clause {
  detach: true;
  expressions: Expression[];
}

export interface Remove extends Clause {
  items: Expression[];
}

export interface RemoveItem extends AstNode {}

export interface RemoveLabels extends RemoveItem {
  identifier: Identifier;
  labels: Label[];
}

export interface RemoveProperty extends RemoveItem {
  property: PropertyOperator;
}

export interface ForEach extends Clause {
  identifier: Identifier;
  expression: Expression;
  clauses: Clause[];
}

export interface With extends Clause {
  distinct: boolean;
  includeExisting: boolean;
  projections: Projection[];
  orderBy: OrderBy;
  skip: Integer;
  limit: Integer;
  predicate: Expression;
}

export interface Unwind extends Clause {
  expression: Expression;
  alias: Expression;
}

export interface Call extends Clause {
  procName: ProcName;
  args: Expression[];
  projections: Projection[];
}

export interface Return extends Clause {
  distinct: boolean;
  includeExisting: boolean;
  projections: Projection[];
  orderBy: OrderBy;
  skip: Integer;
  limit: Integer;
}

export interface Projection extends AstNode {
  expression: Expression;
  alias: Identifier;
}

export interface OrderBy extends AstNode {
  items: SortItem[];
}

export interface SortItem extends AstNode {
  expression: Expression;
  ascending: boolean;
}

export interface Union extends Clause {
  all: boolean;
}

export interface Expression extends AstNode {}

export interface UnaryOperator extends Expression {
  op: string;
  arg: Expression;
}

export interface BinaryOperator extends Expression {
  op: string;
  arg1: Expression;
  arg2: Expression;
}

export interface Comparison extends Expression {
  length: number;
  ops: string[];
  args: Expression[];
}

export interface ApplyOperator extends Expression {
  funcName: FunctionName;
  distinct: boolean;
  args: Expression[];
}

export interface ApplyAllOperator extends Expression {
  funcName: FunctionName;
  distinct: boolean;
}

export interface PropertyOperator extends Expression {
  expression: Expression;
  propName: PropName;
}

export interface SubscriptOperator extends Expression {
  expression: Expression;
  subscript: Expression;
}

export interface SliceOperator extends Expression {
  expression: Expression;
  start: Expression;
  end: Expression;
}

export interface MapProjection extends Expression {
  expression: Expression;
  selectors: MapProjectionSelector[];
}

export interface MapProjectionSelector extends AstNode {}

export interface MapProjectionLiteral extends MapProjectionSelector {
  propName: PropName;
  expression: Expression;
}

export interface MapProjectionProperty extends MapProjectionSelector {
  propName: PropName;
}

export interface MapProjectionIdentifier extends MapProjectionSelector {
  identifier: Identifier;
}

export interface MapProjectionAllProperties extends MapProjectionSelector {}

export interface ListComprehension extends Expression {
  identifier: Identifier;
  expression: Expression;
  predicate: Expression;
  eval: Expression;
}

export interface PatternComprehension extends Expression {
  identifier: Identifier;
  pattern: PatternPath;
  predicate: Expression;
  eval: Expression;
}

export interface Alternative {
  predicate: Expression;
  value: Expression;
}

export interface Case extends Expression {
  expression: Expression;
  alternatives: Alternative[];
  default: Expression;
}

export interface Filter extends ListComprehension {
  identifier: Identifier;
  expression: Expression;
  predicate: Expression;
}

export interface Extract extends ListComprehension {
  identifier: Identifier;
  expression: Expression;
  eval: Expression;
}

export interface Reduce extends Expression {
  accumulator: Identifier;
  init: Expression;
  identifier: Identifier;
  expression: Expression;
  eval: Expression;
}

export interface All extends ListComprehension {
  identifier: Identifier;
  expression: Expression;
  predicate: Expression;
}

export interface Any extends ListComprehension {
  identifier: Identifier;
  expression: Expression;
  predicate: Expression;
}

export interface Single extends ListComprehension {
  identifier: Identifier;
  expression: Expression;
  predicate: Expression;
}

export interface None extends ListComprehension {
  identifier: Identifier;
  expression: Expression;
  predicate: Expression;
}

export interface Collection extends Expression {
  elements: AstNode;
}

export interface Map extends Expression {
  entries: { [name: string]: AstNode };
}

export interface Identifier extends Expression {
  name: string;
}

export interface String extends Expression {
  value: string;
}

export interface Integer extends Expression {
  value: number;
}

export interface Float extends Expression {
  value: number;
}

export interface Boolean extends Expression {}
export interface True extends Boolean {}
export interface False extends Boolean {}
export interface Null extends Expression {}

export interface Label extends AstNode {
  name: string;
}

export interface RelType extends AstNode {
  name: string;
}

export interface PropName extends AstNode {
  value: string;
}

export interface FunctionName extends AstNode {
  value: string;
}

export interface IndexName extends AstNode {
  value: string;
}

export interface ProcName extends AstNode {
  value: string;
}

export interface Pattern extends AstNode {
  paths: PatternPath[];
}

export type patternPathElement = NodePattern | RelPattern;

export interface PatternPath extends AstNode {
  elements: patternPathElement[];
}

export interface NamedPath extends PatternPath {
  identifier: Identifier;
  path: PatternPath;
}

export interface ShortestPath extends PatternPath {
  single: boolean;
  path: PatternPath;
}

export interface NodePattern extends AstNode {
  identifier: Identifier;
  labels: Label[];
  properties: Map|Parameter;
}

export interface RelPattern extends AstNode {
  direction: number;
  identifier: Identifier;
  relTypes: Label[];
  properties: Map|Parameter;
  varLength: Range;
}

export interface Range extends AstNode {
  start: number;
  end: number;
}

export interface Command extends AstNode {
  name: string;
  args: String[];
}

export interface Comment extends AstNode {
  value: string;
}

export interface LineComment extends Comment {}

export interface BlockComment extends Comment {}

export interface Error extends AstNode {
  value: string;
}