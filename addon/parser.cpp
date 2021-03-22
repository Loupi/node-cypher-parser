#include "parser.hpp"
#include <iostream>
#include <exception>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "memstream/memstream.h"
#include "fmemopen/fmemopen.h"

const std::string GetJsonText(const rapidjson::Value& doc)
{
  rapidjson::StringBuffer buffer;
  buffer.Clear();

  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return std::string(buffer.GetString());
}

unsigned int NodeBin::LoopErrors(const cypher_parse_result_t* parseResult) const {
  rapidjson::Value nodes(rapidjson::Type::kArrayType);
  auto nErrors = cypher_parse_result_nerrors(parseResult);

  for (unsigned int i = 0; i < nErrors; i++) {
    auto node = cypher_parse_result_get_error(parseResult, i);
    if (!node)
      continue;
    
    rapidjson::Value nodeTree(rapidjson::kObjectType);
    auto bin = NodeBin((const cypher_astnode_t*)node, nodeTree, allocator);
    auto position = cypher_parse_error_position(node);
    rapidjson::Value nodeTreePos(rapidjson::kObjectType);
    auto binPos = NodeBin((const cypher_astnode_t*)node, nodeTreePos, allocator);
    binPos.AddMember("line", (int)position.line);
    binPos.AddMember("column", (int)position.column);
    binPos.AddMember("offset", (int)position.offset);
    bin.AddMember("position", nodeTreePos);
    bin.AddMember("message", std::string(cypher_parse_error_message(node)).c_str());
    bin.AddMember("context", cypher_parse_error_context(node));
    bin.AddMember("contextOffset", (int)cypher_parse_error_context_offset(node));
    nodes.PushBack(nodeTree, allocator);
  }
  
  AddMember("errors", nodes);

  return nErrors;
}

void NodeBin::GetAst(const cypher_parse_result_t* parseResult, unsigned int width,
                       const struct cypher_parser_colorization *colorization, uint_fast32_t flags, std::string& str) {
  char *buf;
  size_t len;
  FILE *stream = open_memstream(&buf, &len);
  auto error = cypher_parse_result_fprint_ast(parseResult, stream, width, colorization, flags);
  fflush(stream);
  if (!error) {
    str = std::string(buf);
  }
  fclose (stream);
  free(buf);
}

bool NodeBin::Parse(std::string& json, std::string& query, unsigned int width, bool dumpAst, bool colorize, bool parseOnlyStatements) {
  auto config = cypher_parser_new_config();
  uint_fast32_t flags = parseOnlyStatements ? CYPHER_PARSE_ONLY_STATEMENTS : 0;
  if (config == NULL) {
    std::cerr << "cypher_parser_new_config" << std::endl;
    return false; 
  }

  auto colorization = cypher_parser_no_colorization;
  if (colorize) {
    colorization = cypher_parser_ansi_colorization;
    cypher_parser_config_set_error_colorization(config, colorization);
  }

  FILE *stream = fmemopen((void*)query.c_str(), query.length(), "r");
  auto parseResult = cypher_fparse(stream, NULL, config, flags);
  if (parseResult == NULL) {
    fclose(stream);
    std::cerr << "cypher_parse" << std::endl;
    return false;
  }
  fclose(stream);

  auto nErrors = cypher_parse_result_nerrors(parseResult);    

  std::string ast;
  if (!nErrors && dumpAst)
    GetAst(parseResult, width, colorization, flags, ast);
  
  rapidjson::Document document(rapidjson::kObjectType);
  rapidjson::Value result(rapidjson::kObjectType);
  auto bin = NodeBin((const cypher_astnode_t*)parseResult, result, document.GetAllocator());

  bin.AddMember("eof", (bool)cypher_parse_result_eof(parseResult));
  bin.LoopNodes("roots", (node_counter)cypher_parse_result_nroots, (node_getter)cypher_parse_result_get_root);
  bin.LoopNodes("directives", (node_counter)cypher_parse_result_ndirectives, (node_getter)cypher_parse_result_get_directive);
  bin.AddMember("nnodes", (int)cypher_parse_result_nnodes(parseResult));
  bin.LoopErrors(parseResult);
  if (nErrors && dumpAst)
    GetAst(parseResult, width, colorization, flags, ast);

  if (dumpAst)
    bin.AddMember("ast", ast.c_str());
  
  cypher_parse_result_free(parseResult);
  cypher_parser_config_free(config);

  json = GetJsonText(result);

  return nErrors == 0;
}

NodeBin::NodeBin(const cypher_astnode_t *n, rapidjson::Value& p, rapidjson::Document::AllocatorType& a):
    node(n),
    parent(p),
    allocator(a) {}

void NodeBin::AddMember(const char* key, const char* value) const {
  rapidjson::Value k(key, allocator);
  rapidjson::Value v(value, allocator);
  parent.AddMember(k, v, allocator);
}

void NodeBin::AddMember(const char* key, int value) const {
  rapidjson::Value k(key, allocator);
  parent.AddMember(k, rapidjson::Value().SetInt(value), allocator);
}

void NodeBin::AddMember(const char* key, bool value) const {
  rapidjson::Value k(key, allocator);
  parent.AddMember(k, rapidjson::Value().SetBool(value), allocator);
}

void NodeBin::AddMember(const char* key, rapidjson::Value& value) const {
  rapidjson::Value k(key, allocator);
  parent.AddMember(k, value, allocator);
}

void NodeBin::AddMemberInt(const char* key, specific_node_getter getter) const {
  AddMemberInt(key, getter(node));
}

void NodeBin::AddMemberInt(const char* key, const cypher_astnode_t* intNode) const {
  if (!intNode) {
    AddMemberNull(key);
    return;
  }
  
  auto strVal = cypher_ast_integer_get_valuestr(intNode);
  if (!strVal) {
    AddMemberNull(key);
    return;
  }

  try {
    auto i = atoi(strVal);
    AddMember(key, i);
  }
  catch (const std::exception& e) {
    std::cerr << "std::stoi exception: " << e.what() << std::endl;
    AddMemberNull(key);
  }
}

void NodeBin::AddMemberFloat(const char* key, specific_node_getter getter) const {
  AddMemberFloat(key, getter(node));
}

void NodeBin::AddMemberFloat(const char* key, const cypher_astnode_t* floatNode) const {
  if (!floatNode) {
    AddMemberNull(key);
    return;
  }
  
  auto strVal = cypher_ast_float_get_valuestr(floatNode);
  if (!strVal) {
    AddMemberNull(key);
    return;
  }

  try {
    auto ld = strtod(strVal, NULL);
    rapidjson::Value value;
    value.SetDouble(ld);
    AddMember(key, value);
  }
  catch (const std::exception& e) {
    std::cerr << "std::stod exception: " << e.what() << std::endl;
    AddMemberNull(key);
  }
}

void NodeBin::AddMemberStr(const char* key, specific_node_getter getter) const {
  auto strNode = getter(node);
  if (!strNode) {
    AddMemberNull(key);
    return;
  }
  
  auto strVal = cypher_ast_string_get_value(strNode);
  if (!strVal) {
    AddMemberNull(key);
    return;
  }

  AddMember(key, strVal);
}

void NodeBin::AddMemberNull(const char* key) const {
  rapidjson::Value k(key, allocator);
  rapidjson::Value v;
  v.SetNull();
  parent.AddMember(k, v, allocator);
}

const char* NodeBin::ParseOp(const cypher_operator_t* op) const {
  if (op == CYPHER_OP_OR)
    return "or";
  else if (op == CYPHER_OP_XOR)
    return "xor";
  else if (op == CYPHER_OP_AND)
    return "and";
  else if (op == CYPHER_OP_NOT)
    return "not";
  else if (op == CYPHER_OP_EQUAL)
    return "equal";
  else if (op == CYPHER_OP_NEQUAL)
    return "not-equal";
  else if (op == CYPHER_OP_LT)
    return "less-than";
  else if (op == CYPHER_OP_GT)
    return "greater-than";
  else if (op == CYPHER_OP_LTE)
    return "less-than-equal";
  else if (op == CYPHER_OP_GTE)
    return "greater-than-equal";
  else if (op == CYPHER_OP_PLUS)
    return "plus";
  else if (op == CYPHER_OP_MINUS)
    return "minus";
  else if (op == CYPHER_OP_MULT)
    return "mult";
  else if (op == CYPHER_OP_DIV)
    return "div";
  else if (op == CYPHER_OP_MOD)
    return "mod";
  else if (op == CYPHER_OP_POW)
    return "pow";
  else if (op == CYPHER_OP_UNARY_PLUS)
    return "unary-plus";
  else if (op == CYPHER_OP_UNARY_MINUS)
    return "unary-minus";
  else if (op == CYPHER_OP_SUBSCRIPT)
    return "subscript";
  else if (op == CYPHER_OP_MAP_PROJECTION)
    return "map-projection";
  else if (op == CYPHER_OP_REGEX)
    return "regex";
  else if (op == CYPHER_OP_IN)
    return "in";
  else if (op == CYPHER_OP_STARTS_WITH)
    return "starts-with";
  else if (op == CYPHER_OP_ENDS_WITH)
    return "ends-with";
  else if (op == CYPHER_OP_CONTAINS)
    return "contains";
  else if (op == CYPHER_OP_IS_NULL)
    return "is-null";
  else if (op == CYPHER_OP_IS_NOT_NULL)
    return "is-not-null";
  else if (op == CYPHER_OP_PROPERTY)
    return "property";
  else if (op == CYPHER_OP_LABEL)
    return "label";
  else {
    std::cerr << "WARNING: Unknown operator" << std::endl;
    return NULL;
  }
}

void NodeBin::AddMemberOp(const char* key, operator_getter getter) const {
  AddMember(key, ParseOp(getter(node)));
}

void NodeBin::LoopOps(const char* name, node_counter counter, op_getter getter) const {
  rapidjson::Value nodes(rapidjson::Type::kArrayType);
  for (unsigned int i = 0; i < counter(node); i++) {
    rapidjson::Value op(ParseOp(getter(node, i)), allocator);
    nodes.PushBack(op, allocator);
  }
  AddMember(name, nodes);
}

void NodeBin::LoopNodes(const char* name, unsigned int counter, node_getter getter) const {
  rapidjson::Value nodes(rapidjson::Type::kArrayType);

  for (unsigned int i = 0; i < counter; i++) {
    auto node = getter(this->node, i);
    if (!node)
      continue;
    
    rapidjson::Value nodeTree(rapidjson::kObjectType);
    auto bin = NodeBin(node, nodeTree, allocator);
    bin.WalkNode(0);
    nodes.PushBack(nodeTree, allocator);
  }
  
  AddMember(name, nodes);
}

void NodeBin::LoopNodes(const char* name, node_counter counter, node_getter getter) const {
  LoopNodes(name, counter(node), getter);
}

void NodeBin::LoopKeyValuePairs(const char* name, const char* keyName, const char* valueName,
                                node_counter counter, node_getter keyGetter, node_getter valueGetter) const {
  rapidjson::Value nodes(rapidjson::Type::kArrayType);
  
  for (unsigned int i = 0; i < counter(node); i++) {
    auto key = keyGetter(node, i);
    if (!key)
      continue;

    auto value = valueGetter(node, i);
    if (!value)
      continue;
    
    rapidjson::Value nodeTree(rapidjson::kObjectType);
    auto bin = NodeBin(node, nodeTree, allocator);
    bin.Node(keyName, key);
    bin.Node(valueName, value);
    nodes.PushBack(nodeTree, allocator);
  }
  
  AddMember(name, nodes);
}

void NodeBin::Node(const char* name, const cypher_astnode_t* node) const {
  if (!node)
    return;
  
  rapidjson::Value nodeTree(rapidjson::kObjectType);
  auto bin = NodeBin(node, nodeTree, allocator);
  bin.WalkNode(0);
  AddMember(name, nodeTree);
}

void NodeBin::Node(const char* name, specific_node_getter getter) const {
  Node(name, getter(this->node));
}

void NodeBin::SwitchWalk(cypher_astnode_type_t nodeType) const {
  if (nodeType == CYPHER_AST_STATEMENT)
    WalkStatement();
  else if (nodeType == CYPHER_AST_STATEMENT_OPTION)
    WalkStatementOption();
  else if (nodeType == CYPHER_AST_CYPHER_OPTION)
    WalkCypherOption();
  else if (nodeType == CYPHER_AST_CYPHER_OPTION_PARAM)
    WalkCypherOptionParam();
  else if (nodeType == CYPHER_AST_EXPLAIN_OPTION)
    WalkExplainOption();
  else if (nodeType == CYPHER_AST_PROFILE_OPTION)
    WalkProfileOption();

  /*******************CYPHER_AST_SCHEMA_COMMAND**********************/
  else if (nodeType == CYPHER_AST_CREATE_NODE_PROP_INDEX)
    WalkCreateNodePropIndex();
  else if (nodeType == CYPHER_AST_DROP_NODE_PROP_INDEX)
    WalkDropNodePropIndex();
  else if (nodeType == CYPHER_AST_CREATE_NODE_PROP_CONSTRAINT)
    WalkCreateNodePropConstraint();
  else if (nodeType == CYPHER_AST_DROP_NODE_PROP_CONSTRAINT)
    WalkDropNodePropConstraint();
  else if (nodeType == CYPHER_AST_CREATE_REL_PROP_CONSTRAINT)
    WalkCreateRelPropConstraint();
  else if (nodeType == CYPHER_AST_DROP_REL_PROP_CONSTRAINT)
    WalkDropRelPropConstraint();

  else if (nodeType == CYPHER_AST_QUERY)
    WalkQuery();
  
  /*******************CYPHER_AST_QUERY_OPTION**********************/
  else if (nodeType == CYPHER_AST_USING_PERIODIC_COMMIT)
    WalkUsingPeriodicCommit();
   
  /*******************CYPHER_AST_QUERY_CLAUSE**********************/
  else if (nodeType == CYPHER_AST_LOAD_CSV)
    WalkLoadCsv();
  else if (nodeType == CYPHER_AST_START)
    WalkStart();

  /********************CYPHER_AST_START_POINT**********************/
  else if (nodeType == CYPHER_AST_NODE_INDEX_LOOKUP)
    WalkNodeIndexLookup();
  else if (nodeType == CYPHER_AST_NODE_INDEX_QUERY)
    WalkNodeIndexQuery();
  else if (nodeType == CYPHER_AST_NODE_ID_LOOKUP)
    WalkNodeIdLookup();
  else if (nodeType == CYPHER_AST_ALL_NODES_SCAN)
    WalkAllNodesScan();
  else if (nodeType == CYPHER_AST_REL_INDEX_LOOKUP)
    WalkRelIndexLookup();
  else if (nodeType == CYPHER_AST_REL_INDEX_QUERY)
    WalkRelIndexQuery();
  else if (nodeType == CYPHER_AST_REL_ID_LOOKUP)
    WalkRelIdLookup();
  else if (nodeType == CYPHER_AST_ALL_RELS_SCAN)
    WalkAllRelsScan();
  
  else if (nodeType == CYPHER_AST_MATCH)
    WalkMatch();

  /********************CYPHER_AST_MATCH_HINT***********************/
  else if (nodeType == CYPHER_AST_USING_INDEX)
    WalkUsingIndex();
  else if (nodeType == CYPHER_AST_USING_JOIN)
    WalkUsingJoin();
  else if (nodeType == CYPHER_AST_USING_SCAN)
    WalkUsingScan();
  
  else if (nodeType == CYPHER_AST_MERGE)
    WalkMerge();

  /*******************CYPHER_AST_MERGE_ACTION**********************/
  else if (nodeType == CYPHER_AST_ON_MATCH)
    WalkOnMatch();
  else if (nodeType == CYPHER_AST_ON_CREATE)
    WalkOnCreate();
  
  else if (nodeType == CYPHER_AST_CREATE)
    WalkCreate();

  else if (nodeType == CYPHER_AST_SET)
    WalkSet();

  /*********************CYPHER_AST_SET_ITEM************************/
  else if (nodeType == CYPHER_AST_SET_PROPERTY)
    WalkSetProperty();
  else if (nodeType == CYPHER_AST_SET_ALL_PROPERTIES)
    WalkSetAllProperties();
  else if (nodeType == CYPHER_AST_MERGE_PROPERTIES)
    WalkMergeProperties();
  else if (nodeType == CYPHER_AST_SET_LABELS)
    WalkSetLabels();
  
  else if (nodeType == CYPHER_AST_DELETE)
    WalkDelete();

  else if (nodeType == CYPHER_AST_REMOVE)
    WalkRemove();

  /********************CYPHER_AST_REMOVE_ITEM**********************/
  else if (nodeType == CYPHER_AST_REMOVE_LABELS)
    WalkRemoveLabels();
  else if (nodeType == CYPHER_AST_REMOVE_PROPERTY)
    WalkRemoveProperty();

  else if (nodeType == CYPHER_AST_FOREACH)
    WalkForEach();
  else if (nodeType == CYPHER_AST_WITH)
    WalkWith();
  else if (nodeType == CYPHER_AST_UNWIND)
    WalkUnwind();
  else if (nodeType == CYPHER_AST_CALL)
    WalkCall();
  else if (nodeType == CYPHER_AST_RETURN)
    WalkReturn();
  else if (nodeType == CYPHER_AST_PROJECTION)
    WalkProjection();
  else if (nodeType == CYPHER_AST_ORDER_BY)
    WalkOrderBy();
  else if (nodeType == CYPHER_AST_SORT_ITEM)
    WalkSortItem();
  else if (nodeType == CYPHER_AST_UNION)
    WalkUnion();

  /*CYPHER_AST_EXPRESSION*/
  else if (nodeType == CYPHER_AST_UNARY_OPERATOR)
    WalkUnaryOperator();
  else if (nodeType == CYPHER_AST_BINARY_OPERATOR)
    WalkBinaryOperator();
  else if (nodeType == CYPHER_AST_COMPARISON)
    WalkComparison();
  else if (nodeType == CYPHER_AST_APPLY_OPERATOR)
    WalkApplyOperator();
  else if (nodeType == CYPHER_AST_APPLY_ALL_OPERATOR)
    WalkApplyAllOperator();
  else if (nodeType == CYPHER_AST_PROPERTY_OPERATOR)
    WalkPropertyOperator();
  else if (nodeType == CYPHER_AST_SUBSCRIPT_OPERATOR)
    WalkSubscriptOperator();
  else if (nodeType == CYPHER_AST_SLICE_OPERATOR)
    WalkSliceOperator();

  else if (nodeType == CYPHER_AST_MAP_PROJECTION)
    WalkMapProjection();
  
  /****************CYPHER_AST_MAP_PROJECTION_SELECTOR**************/
  else if (nodeType == CYPHER_AST_MAP_PROJECTION_LITERAL)
    WalkMapProjectionLiteral();
  else if (nodeType == CYPHER_AST_MAP_PROJECTION_PROPERTY)
    WalkMapProjectionProperty();
  else if (nodeType == CYPHER_AST_MAP_PROJECTION_IDENTIFIER)
    WalkMapProjectionIdentifier();
  else if (nodeType == CYPHER_AST_MAP_PROJECTION_ALL_PROPERTIES)
    WalkMapProjectionAllProperties();
  
  else if (nodeType == CYPHER_AST_LABELS_OPERATOR)
    WalkLabelsOperator();
  
  else if (nodeType == CYPHER_AST_LIST_COMPREHENSION)
    WalkListComprehension();
  else if (nodeType == CYPHER_AST_PATTERN_COMPREHENSION)
    WalkPatternComprehension();
  else if (nodeType == CYPHER_AST_CASE)
    WalkCase();
  else if (nodeType == CYPHER_AST_FILTER)
    WalkFilter();
  else if (nodeType == CYPHER_AST_EXTRACT)
    WalkExtract();
  else if (nodeType == CYPHER_AST_REDUCE)
    WalkReduce();
  else if (nodeType == CYPHER_AST_ALL)
    WalkAll();
  else if (nodeType == CYPHER_AST_ANY)
    WalkAny();
  else if (nodeType == CYPHER_AST_SINGLE)
    WalkSingle();
  else if (nodeType == CYPHER_AST_NONE)
    WalkNone();
  else if (nodeType == CYPHER_AST_COLLECTION)
    WalkCollection();
  else if (nodeType == CYPHER_AST_MAP)
    WalkMap();
  else if (nodeType == CYPHER_AST_IDENTIFIER)
    WalkIdentifier();
  else if (nodeType == CYPHER_AST_PARAMETER)
    WalkParameter();
  else if (nodeType == CYPHER_AST_STRING)
    WalkString();
  else if (nodeType == CYPHER_AST_INTEGER)
    WalkInteger();
  else if (nodeType == CYPHER_AST_FLOAT)
    WalkFloat();
  /*else if (nodeType == CYPHER_AST_BOOLEAN)
    WalkBoolean();*/
  else if (nodeType == CYPHER_AST_TRUE)
    WalkTrue();
  else if (nodeType == CYPHER_AST_FALSE)
    WalkFalse();
  else if (nodeType == CYPHER_AST_NULL)
    WalkNull();
  else if (nodeType == CYPHER_AST_LABEL)
    WalkLabel();
  else if (nodeType == CYPHER_AST_RELTYPE)
    WalkRelType();
  else if (nodeType == CYPHER_AST_PROP_NAME)
    WalkPropName();
  else if (nodeType == CYPHER_AST_FUNCTION_NAME)
    WalkFunctionName();
  else if (nodeType == CYPHER_AST_INDEX_NAME)
    WalkIndexName();
  else if (nodeType == CYPHER_AST_PROC_NAME)
    WalkProcName();
  else if (nodeType == CYPHER_AST_PATTERN)
    WalkPattern();
  else if (nodeType == CYPHER_AST_NAMED_PATH)
    WalkNamedPath();
  else if (nodeType == CYPHER_AST_SHORTEST_PATH)
    WalkShortestPath();
  else if (nodeType == CYPHER_AST_PATTERN_PATH)
    WalkPatternPath();
  else if (nodeType == CYPHER_AST_NODE_PATTERN)
    WalkNodePattern();
  else if (nodeType == CYPHER_AST_REL_PATTERN)
    WalkRelPattern();
  else if (nodeType == CYPHER_AST_RANGE)
    WalkRange();
  else if (nodeType == CYPHER_AST_COMMAND)
    WalkCommand();
  /*CYPHER_AST_COMMENT*/
  else if (nodeType == CYPHER_AST_LINE_COMMENT)
    WalkLineComment();
  else if (nodeType == CYPHER_AST_BLOCK_COMMENT)
    WalkBlockComment();
  else if (nodeType == CYPHER_AST_ERROR)
    WalkError();
  else
    std::cerr << "WARNING: No walker" << std::endl;
}

void NodeBin::WalkNode(int nodeOffset) const {
  auto nodeType = cypher_astnode_type(node);
  SwitchWalk(nodeType);
}

void NodeBin::WalkParameter() const {
  AddMember("type", "parameter");
  AddMember("name", cypher_ast_parameter_get_name(node));
}

void NodeBin::WalkMatch() const {
  AddMember("type", "match");
  AddMember("optional", (bool)cypher_ast_match_is_optional(node));
  Node("pattern", cypher_ast_match_get_pattern);
  LoopNodes("hints", cypher_ast_match_nhints, cypher_ast_match_get_hint);
  Node("predicate", cypher_ast_match_get_predicate);
}

void NodeBin::WalkQuery() const {
  AddMember("type", "query");
  LoopNodes("clauses", cypher_ast_query_nclauses, cypher_ast_query_get_clause);
  LoopNodes("options", cypher_ast_query_noptions, cypher_ast_query_get_option);
}

void NodeBin::WalkStatement() const {
  AddMember("type", "statement");
  Node("body", cypher_ast_statement_get_body);
  LoopNodes("options", cypher_ast_statement_noptions, cypher_ast_statement_get_option);
}

void NodeBin::WalkStatementOption() const {
  AddMember("type", "statement-option");
}

void NodeBin::WalkCypherOption() const {
  AddMember("type", "cypher-option");
  AddMember("version", cypher_ast_cypher_option_get_version(node));
  LoopNodes("params", cypher_ast_cypher_option_nparams, cypher_ast_cypher_option_get_param);
}

void NodeBin::WalkCypherOptionParam() const {
  AddMember("type", "cypher-option-param");
  Node("name", cypher_ast_cypher_option_param_get_name);
  Node("value", cypher_ast_cypher_option_param_get_value(node));
}

void NodeBin::WalkExplainOption() const {
  WalkStatementOption();
}

void NodeBin::WalkProfileOption() const {
  WalkStatementOption();
}

void NodeBin::WalkCreateNodePropIndex() const {
  AddMember("type", "create-node-prop-index");
  Node("label", cypher_ast_create_node_prop_index_get_label);
  Node("propName", cypher_ast_create_node_prop_index_get_prop_name);
}

void NodeBin::WalkDropNodePropIndex() const {
  AddMember("type", "drop-node-prop-index");
  Node("label", cypher_ast_drop_node_prop_index_get_label);
  Node("propName", cypher_ast_drop_node_prop_index_get_prop_name);
}

void NodeBin::WalkCreateNodePropConstraint() const {
  AddMember("type", "create-node-prop-constraint");
  Node("identifier", cypher_ast_create_node_prop_constraint_get_identifier);
  Node("label", cypher_ast_create_node_prop_constraint_get_label);
  Node("expression", cypher_ast_create_node_prop_constraint_get_expression);
  AddMember("unique", (bool)cypher_ast_create_node_prop_constraint_is_unique(node));
}

void NodeBin::WalkDropNodePropConstraint() const {
  AddMember("type", "drop-node-prop-constraint");
  Node("identifier", cypher_ast_drop_node_prop_constraint_get_identifier);
  Node("label", cypher_ast_drop_node_prop_constraint_get_label);
  Node("expression", cypher_ast_drop_node_prop_constraint_get_expression);
  AddMember("unique", (bool)cypher_ast_drop_node_prop_constraint_is_unique(node));
}

void NodeBin::WalkCreateRelPropConstraint() const {
  AddMember("type", "create-rel-prop-constraint");
  Node("identifier", cypher_ast_create_rel_prop_constraint_get_identifier);
  Node("relType", cypher_ast_create_rel_prop_constraint_get_reltype);
  Node("expression", cypher_ast_create_rel_prop_constraint_get_expression);
  AddMember("unique", (bool)cypher_ast_create_rel_prop_constraint_is_unique(node));
}

void NodeBin::WalkDropRelPropConstraint() const {
  AddMember("type", "drop-rel-prop-constraint");
  Node("identifier", cypher_ast_drop_rel_prop_constraint_get_identifier);
  Node("relType", cypher_ast_drop_rel_prop_constraint_get_reltype);
  Node("expression", cypher_ast_drop_rel_prop_constraint_get_expression);
  AddMember("unique", (bool)cypher_ast_drop_rel_prop_constraint_is_unique(node));
}

void NodeBin::WalkUsingPeriodicCommit() const {
  AddMember("type", "using-periodic-commit");
  AddMemberInt("limit", cypher_ast_using_periodic_commit_get_limit);
}

void NodeBin::WalkLoadCsv() const {
  AddMember("type", "load-csv");
  AddMember("withHeaders", (bool)cypher_ast_load_csv_has_with_headers(node));
  Node("url", cypher_ast_load_csv_get_url);
  Node("identifier", cypher_ast_load_csv_get_identifier);
  Node("fieldTerminator", cypher_ast_load_csv_get_field_terminator);
}

void NodeBin::WalkStart() const {
  AddMember("type", "start");
  LoopNodes("points", cypher_ast_start_npoints, cypher_ast_start_get_point);
  Node("predicate", cypher_ast_start_get_predicate);
}

void NodeBin::WalkNodeIndexLookup() const {
  AddMember("type", "node-index-lookup");
  Node("identifier", cypher_ast_node_index_lookup_get_identifier);
  Node("indexName", cypher_ast_node_index_lookup_get_index_name);
  Node("propName", cypher_ast_node_index_lookup_get_prop_name);
  Node("lookup", cypher_ast_node_index_lookup_get_lookup);
}

void NodeBin::WalkNodeIndexQuery() const {
  AddMember("type", "node-index-query");
  Node("identifier", cypher_ast_node_index_query_get_identifier);
  Node("indexName", cypher_ast_node_index_query_get_index_name);
  Node("query", cypher_ast_node_index_query_get_query);
}

void NodeBin::WalkNodeIdLookup() const {
  AddMember("type", "node-id-lookup");
  Node("identifier", cypher_ast_node_id_lookup_get_identifier);
  LoopNodes("ids", cypher_ast_node_id_lookup_nids, cypher_ast_node_id_lookup_get_id);
}

void NodeBin::WalkAllNodesScan() const {
  AddMember("type", "all-nodes-scan");
  Node("identifier", cypher_ast_all_nodes_scan_get_identifier);
}

void NodeBin::WalkRelIndexLookup() const {
  AddMember("type", "rel-index-lookup");
  Node("identifier", cypher_ast_rel_index_lookup_get_identifier);
  Node("indexName", cypher_ast_rel_index_lookup_get_index_name);
  Node("propName", cypher_ast_rel_index_lookup_get_prop_name);
  Node("lookup", cypher_ast_rel_index_lookup_get_lookup);
}

void NodeBin::WalkRelIndexQuery() const {
  AddMember("type", "rel-index-query");
  Node("identifier", cypher_ast_rel_index_query_get_identifier);
  Node("indexName", cypher_ast_rel_index_query_get_index_name);
  Node("query", cypher_ast_rel_index_query_get_query);
}

void NodeBin::WalkRelIdLookup() const {
  AddMember("type", "rel-id-lookup");
  Node("identifier", cypher_ast_rel_id_lookup_get_identifier);
  LoopNodes("ids", cypher_ast_rel_id_lookup_nids, cypher_ast_rel_id_lookup_get_id);
}

void NodeBin::WalkAllRelsScan() const {
  AddMember("type", "all-rels-scan");
  Node("identifier", cypher_ast_all_rels_scan_get_identifier);
}

void NodeBin::WalkUsingIndex() const {
  AddMember("type", "using-index");
  Node("identifier", cypher_ast_using_index_get_identifier);
  Node("label", cypher_ast_using_index_get_label);
  Node("propName", cypher_ast_using_index_get_prop_name);
}

void NodeBin::WalkUsingJoin() const {
  AddMember("type", "using-join");
  LoopNodes("identifiers", cypher_ast_using_join_nidentifiers, cypher_ast_using_join_get_identifier);
}

void NodeBin::WalkUsingScan() const {
  AddMember("type", "using-scan");
  Node("identifier", cypher_ast_using_scan_get_identifier);
  Node("label", cypher_ast_using_scan_get_label);
}

void NodeBin::WalkMerge() const {
  AddMember("type", "merge");
  Node("path", cypher_ast_merge_get_pattern_path);
  LoopNodes("actions", cypher_ast_merge_nactions, cypher_ast_merge_get_action);
}

void NodeBin::WalkOnMatch() const {
  AddMember("type", "on-match");
  LoopNodes("items", cypher_ast_on_match_nitems, cypher_ast_on_match_get_item);
}

void NodeBin::WalkOnCreate() const {
  AddMember("type", "on-create");
  LoopNodes("items", cypher_ast_on_create_nitems, cypher_ast_on_create_get_item);
}

void NodeBin::WalkCreate() const {
  AddMember("type", "create");
  AddMember("unique", (bool)cypher_ast_create_is_unique(node));
  Node("pattern", cypher_ast_create_get_pattern);
}

void NodeBin::WalkSet() const {
  AddMember("type", "set");
  LoopNodes("items", cypher_ast_set_nitems, cypher_ast_set_get_item);
}

void NodeBin::WalkSetProperty() const {
  AddMember("type", "set-property");
  Node("property", cypher_ast_set_property_get_property);
  Node("expression", cypher_ast_set_property_get_expression);
}

void NodeBin::WalkSetAllProperties() const {
  AddMember("type", "set-all-properties");
  Node("identifier", cypher_ast_set_all_properties_get_identifier);
  Node("expression", cypher_ast_set_all_properties_get_expression);
}

void NodeBin::WalkMergeProperties() const {
  AddMember("type", "merge-properties");
  Node("identifier", cypher_ast_merge_properties_get_identifier);
  Node("expression", cypher_ast_merge_properties_get_expression);
}

void NodeBin::WalkSetLabels() const {
  AddMember("type", "set-labels");
  Node("identifier", cypher_ast_set_labels_get_identifier);
  LoopNodes("labels", cypher_ast_set_labels_nlabels, cypher_ast_set_labels_get_label);
}

void NodeBin::WalkDelete() const {
  AddMember("type", "delete");
  AddMember("detach", (bool)cypher_ast_delete_has_detach(node));
  LoopNodes("expressions", cypher_ast_delete_nexpressions, cypher_ast_delete_get_expression);
}

void NodeBin::WalkRemove() const {
  AddMember("type", "remove");
  LoopNodes("items", cypher_ast_remove_nitems, cypher_ast_remove_get_item);
}

void NodeBin::WalkRemoveLabels() const {
  AddMember("type", "remove-labels");
  Node("identifier", cypher_ast_remove_labels_get_identifier);
  LoopNodes("labels", cypher_ast_remove_labels_nlabels, cypher_ast_remove_labels_get_label);
}

void NodeBin::WalkRemoveProperty() const {
  AddMember("type", "remove-property");
  Node("property", cypher_ast_remove_property_get_property);
}

void NodeBin::WalkForEach() const {
  AddMember("type", "for-each");
  Node("identifier", cypher_ast_foreach_get_identifier);
  Node("expression", cypher_ast_foreach_get_expression);
  LoopNodes("clauses", cypher_ast_foreach_nclauses, cypher_ast_foreach_get_clause);
}

void NodeBin::WalkWith() const {
  AddMember("type", "with");
  AddMember("distinct", (bool)cypher_ast_with_is_distinct(node));
  AddMember("includeExisting", (bool)cypher_ast_with_has_include_existing(node));
  LoopNodes("projections", cypher_ast_with_nprojections, cypher_ast_with_get_projection);
  Node("orderBy", cypher_ast_with_get_order_by);
  Node("skip", cypher_ast_with_get_skip);
  Node("limit", cypher_ast_with_get_limit);
  Node("predicate", cypher_ast_with_get_predicate);
}

void NodeBin::WalkUnwind() const {
  AddMember("type", "unwind");
  Node("expression", cypher_ast_unwind_get_expression);
  Node("alias", cypher_ast_unwind_get_alias);
}

void NodeBin::WalkCall() const {
  AddMember("type", "call");
  Node("procName", cypher_ast_call_get_proc_name);
  LoopNodes("args", cypher_ast_call_narguments, cypher_ast_call_get_argument);
  LoopNodes("projections", cypher_ast_call_nprojections, cypher_ast_call_get_projection);
}

void NodeBin::WalkReturn() const {
  AddMember("type", "return");
  AddMember("distinct", (bool)cypher_ast_return_is_distinct(node));
  AddMember("includeExisting", (bool)cypher_ast_return_has_include_existing(node));
  LoopNodes("projections", cypher_ast_return_nprojections, cypher_ast_return_get_projection);
  Node("orderBy", cypher_ast_return_get_order_by);
  Node("skip", cypher_ast_return_get_skip);
  Node("limit", cypher_ast_return_get_limit);
}

void NodeBin::WalkProjection() const {
  AddMember("type", "projection");
  Node("expression", cypher_ast_projection_get_expression);
  Node("alias", cypher_ast_projection_get_alias);
}

void NodeBin::WalkOrderBy() const {
  AddMember("type", "order-by");
  LoopNodes("items", cypher_ast_order_by_nitems, cypher_ast_order_by_get_item);
}

void NodeBin::WalkSortItem() const {
  AddMember("type", "sort-item");
  Node("expression", cypher_ast_sort_item_get_expression);
  AddMember("ascending", (bool)cypher_ast_sort_item_is_ascending(node));
}

void NodeBin::WalkUnion() const {
  AddMember("type", "union");
  AddMember("all", (bool)cypher_ast_union_has_all(node));
}

void NodeBin::WalkUnaryOperator() const {
  AddMember("type", "unary-operator");
  AddMemberOp("op", cypher_ast_unary_operator_get_operator);
  Node("arg", cypher_ast_unary_operator_get_argument);
}

void NodeBin::WalkBinaryOperator() const {
  AddMember("type", "binary-operator");
  Node("arg1", cypher_ast_binary_operator_get_argument1);
  Node("arg2", cypher_ast_binary_operator_get_argument2);
  AddMemberOp("op", cypher_ast_binary_operator_get_operator);
}

void NodeBin::WalkComparison() const {
  AddMember("type", "comparison");
  AddMember("length", (int)cypher_ast_comparison_get_length(node));
  LoopOps("ops", cypher_ast_comparison_get_length, cypher_ast_comparison_get_operator);
  LoopNodes("args", cypher_ast_comparison_get_length(node) + 1, cypher_ast_comparison_get_argument);
}

void NodeBin::WalkApplyOperator() const {
  AddMember("type", "apply-operator");
  Node("funcName", cypher_ast_apply_operator_get_func_name);
  AddMember("distinct", (bool)cypher_ast_apply_operator_get_distinct(node));
  LoopNodes("args", cypher_ast_apply_operator_narguments, cypher_ast_apply_operator_get_argument);
}

void NodeBin::WalkApplyAllOperator() const {
  AddMember("type", "apply-all-operator");
  Node("funcName", cypher_ast_apply_all_operator_get_func_name);
  AddMember("distinct", (bool)cypher_ast_apply_all_operator_get_distinct(node));
}

void NodeBin::WalkPropertyOperator() const {
  AddMember("type", "property-operator");
  Node("expression", cypher_ast_property_operator_get_expression);
  Node("propName", cypher_ast_property_operator_get_prop_name);
}

void NodeBin::WalkSubscriptOperator() const {
  AddMember("type", "subscript-operator");
  Node("expression", cypher_ast_subscript_operator_get_expression);
  Node("subscript", cypher_ast_subscript_operator_get_subscript);
}

void NodeBin::WalkSliceOperator() const {
  AddMember("type", "slice-operator");
  Node("expression", cypher_ast_slice_operator_get_expression);
  Node("start", cypher_ast_slice_operator_get_start);
  Node("end", cypher_ast_slice_operator_get_end);
}

void NodeBin::WalkMapProjection() const {
  AddMember("type", "map-projection");
  Node("expression", cypher_ast_map_projection_get_expression);
  LoopNodes("selectors", cypher_ast_map_projection_nselectors, cypher_ast_map_projection_get_selector);
}

void NodeBin::WalkMapProjectionLiteral() const {
  AddMember("type", "map-projection-literal");
  Node("propName", cypher_ast_map_projection_literal_get_prop_name);
  Node("expression", cypher_ast_map_projection_literal_get_expression);
}

void NodeBin::WalkMapProjectionProperty() const {
  AddMember("type", "map-projection-property");
  Node("propName", cypher_ast_map_projection_property_get_prop_name);
}

void NodeBin::WalkMapProjectionIdentifier() const {
  AddMember("type", "map-projection-identifier");
  Node("identifier", cypher_ast_map_projection_identifier_get_identifier);
}

void NodeBin::WalkMapProjectionAllProperties() const {
  AddMember("type", "map-projection-all-properties");
}

void NodeBin::WalkLabelsOperator() const {
  AddMember("type", "labels-operator");
  Node("expression", cypher_ast_labels_operator_get_expression);
  LoopNodes("labels", cypher_ast_labels_operator_nlabels, cypher_ast_labels_operator_get_label);
}

void NodeBin::WalkListComprehension() const {
  AddMember("type", "list-comprehension");
  Node("identifier", cypher_ast_list_comprehension_get_identifier);
  Node("expression", cypher_ast_list_comprehension_get_expression);
  Node("predicate", cypher_ast_list_comprehension_get_predicate);
  Node("eval", cypher_ast_list_comprehension_get_eval);
}

void NodeBin::WalkPatternComprehension() const {
  AddMember("type", "pattern-comprehension");
  Node("identifier", cypher_ast_pattern_comprehension_get_identifier);
  Node("pattern", cypher_ast_pattern_comprehension_get_pattern);
  Node("predicate", cypher_ast_pattern_comprehension_get_predicate);
  Node("eval", cypher_ast_pattern_comprehension_get_eval);
}

void NodeBin::WalkCase() const {
  AddMember("type", "case");
  Node("expression", cypher_ast_case_get_expression);
  LoopKeyValuePairs("alternatives", "predicate", "value", cypher_ast_case_nalternatives,
                    cypher_ast_case_get_predicate, cypher_ast_case_get_value);
  Node("default", cypher_ast_case_get_default);
}

void NodeBin::WalkFilter() const {
  AddMember("type", "filter");
  Node("identifier", cypher_ast_list_comprehension_get_identifier);
  Node("expression", cypher_ast_list_comprehension_get_expression);
  Node("predicate", cypher_ast_list_comprehension_get_predicate);
}

void NodeBin::WalkExtract() const {
  AddMember("type", "extract");
  Node("identifier", cypher_ast_list_comprehension_get_identifier);
  Node("expression", cypher_ast_list_comprehension_get_expression);
  Node("eval", cypher_ast_pattern_comprehension_get_eval);
}

void NodeBin::WalkReduce() const {
  AddMember("type", "reduce");
  Node("accumulator", cypher_ast_reduce_get_accumulator);
  Node("init", cypher_ast_reduce_get_init);
  Node("identifier", cypher_ast_reduce_get_identifier);
  Node("expression", cypher_ast_reduce_get_expression);
  Node("eval", cypher_ast_reduce_get_eval);
}

void NodeBin::WalkAll() const {
  AddMember("type", "all");
  Node("identifier", cypher_ast_list_comprehension_get_identifier);
  Node("expression", cypher_ast_list_comprehension_get_expression);
  Node("predicate", cypher_ast_list_comprehension_get_predicate);
}

void NodeBin::WalkAny() const {
  AddMember("type", "any");
  Node("identifier", cypher_ast_list_comprehension_get_identifier);
  Node("expression", cypher_ast_list_comprehension_get_expression);
  Node("predicate", cypher_ast_list_comprehension_get_predicate);
}

void NodeBin::WalkSingle() const {
  AddMember("type", "single");
  Node("identifier", cypher_ast_list_comprehension_get_identifier);
  Node("expression", cypher_ast_list_comprehension_get_expression);
  Node("predicate", cypher_ast_list_comprehension_get_predicate);
}

void NodeBin::WalkNone() const {
  AddMember("type", "none");
  Node("identifier", cypher_ast_list_comprehension_get_identifier);
  Node("expression", cypher_ast_list_comprehension_get_expression);
  Node("predicate", cypher_ast_list_comprehension_get_predicate);
}

void NodeBin::WalkCollection() const {
  AddMember("type", "collection");
  LoopNodes("elements", cypher_ast_collection_length, cypher_ast_collection_get);
}

void NodeBin::WalkMap() const {
  AddMember("type", "map");

  rapidjson::Value entries(rapidjson::kObjectType);
  
  for (unsigned int i = 0; i < cypher_ast_map_nentries(node); i++) {
    auto key = cypher_ast_map_get_key(node, i);
    auto value = cypher_ast_map_get_value(node, i);

    if (!key)
      continue;

    auto name = cypher_ast_prop_name_get_value(key);
    rapidjson::Value valueTree(rapidjson::kObjectType);
    auto bin = NodeBin(value, valueTree, allocator);
    bin.WalkNode(0);

    rapidjson::Value k(name, allocator);
    entries.AddMember(k, valueTree, allocator);
  }

  AddMember("entries", entries);
}

void NodeBin::WalkIdentifier() const {
  AddMember("type", "identifier");
  AddMember("name", cypher_ast_identifier_get_name(node));
}

void NodeBin::WalkString() const {
  AddMember("type", "string");
  AddMember("value", cypher_ast_string_get_value(node));
}

void NodeBin::WalkInteger() const {
  AddMember("type", "integer");
  AddMemberInt("value", node);
}

void NodeBin::WalkFloat() const {
  AddMember("type", "float");
  AddMemberFloat("value", node);
}

void NodeBin::WalkTrue() const {
  AddMember("type", "true");
}

void NodeBin::WalkFalse() const {
  AddMember("type", "false");
}

void NodeBin::WalkNull() const {
  AddMember("type", "null");
}

void NodeBin::WalkLabel() const {
  AddMember("type", "label");
  AddMember("name", cypher_ast_label_get_name(node));
}

void NodeBin::WalkRelType() const {
  AddMember("type", "reltype");
  AddMember("name", cypher_ast_reltype_get_name(node));
}

void NodeBin::WalkPropName() const {
  AddMember("type", "prop-name");
  AddMember("value", cypher_ast_prop_name_get_value(node));
}

void NodeBin::WalkFunctionName() const {
  AddMember("type", "function-name");
  AddMember("value", cypher_ast_function_name_get_value(node));
}

void NodeBin::WalkIndexName() const {
  AddMember("type", "index-name");
  AddMember("value", cypher_ast_index_name_get_value(node));
}

void NodeBin::WalkProcName() const {
  AddMember("type", "proc-name");
  AddMember("value", cypher_ast_proc_name_get_value(node));
}

void NodeBin::WalkPattern() const {
  AddMember("type", "pattern");
  LoopNodes("paths", cypher_ast_pattern_npaths, cypher_ast_pattern_get_path);
}

void NodeBin::WalkNamedPath() const {
  AddMember("type", "named-path");
  Node("identifier", cypher_ast_named_path_get_identifier);
  Node("path", cypher_ast_named_path_get_path);
  LoopNodes("elements", cypher_ast_pattern_path_nelements, cypher_ast_pattern_path_get_element);
}

void NodeBin::WalkShortestPath() const {
  AddMember("type", "shortest-path");
  AddMember("single", (bool)cypher_ast_shortest_path_is_single(node));
  Node("path", cypher_ast_shortest_path_get_path);
  LoopNodes("elements", cypher_ast_pattern_path_nelements, cypher_ast_pattern_path_get_element);
}

void NodeBin::WalkPatternPath() const {
  AddMember("type", "pattern-path");
  LoopNodes("elements", cypher_ast_pattern_path_nelements, cypher_ast_pattern_path_get_element);
}

void NodeBin::WalkNodePattern() const {
  AddMember("type", "node-pattern");
  Node("identifier", cypher_ast_node_pattern_get_identifier);
  LoopNodes("labels", cypher_ast_node_pattern_nlabels, cypher_ast_node_pattern_get_label);
  Node("properties", cypher_ast_node_pattern_get_properties);
}

void NodeBin::WalkRelPattern() const {
  AddMember("type", "rel-pattern");
  AddMember("direction", cypher_ast_rel_pattern_get_direction(node));
  Node("identifier", cypher_ast_rel_pattern_get_identifier);
  LoopNodes("reltypes", cypher_ast_rel_pattern_nreltypes, cypher_ast_rel_pattern_get_reltype);
  Node("properties", cypher_ast_rel_pattern_get_properties);
  Node("varLength", cypher_ast_rel_pattern_get_varlength);
}

void NodeBin::WalkRange() const {
  AddMember("type", "range");
  AddMemberInt("start", cypher_ast_range_get_start);
  AddMemberInt("end", cypher_ast_range_get_end);
}

void NodeBin::WalkCommand() const {
  AddMember("type", "command");
  AddMember("name", cypher_ast_command_get_name(node));
  LoopNodes("args", cypher_ast_command_narguments, cypher_ast_command_get_argument);
}

void NodeBin::WalkLineComment() const {
  AddMember("type", "line-comment");
  AddMember("value", cypher_ast_line_comment_get_value(node));
}

void NodeBin::WalkBlockComment() const {
  AddMember("type", "block-comment");
  AddMember("value", cypher_ast_block_comment_get_value(node));
}

void NodeBin::WalkError() const {
  AddMember("type", "error");
  AddMember("value", cypher_ast_error_get_value(node));
}