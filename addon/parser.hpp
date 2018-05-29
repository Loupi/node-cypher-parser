#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <cypher-parser.h>
#include "rapidjson/document.h"

class NodeBin {
public:
  NodeBin(const cypher_astnode_t *n, rapidjson::Value& p, rapidjson::Document::AllocatorType& a);
  void WalkNode(int nodeOffset) const;
  static bool Parse(std::string& json, std::string& query, unsigned int width, bool dumpAst, bool colorize);

private:
  typedef unsigned int (*node_counter)(const cypher_astnode_t *);
  typedef const cypher_astnode_t* (*node_getter)(const cypher_astnode_t *, unsigned int);
  typedef const cypher_astnode_t* (*specific_node_getter)(const cypher_astnode_t *);
  typedef const cypher_operator_t* (*operator_getter)(const cypher_astnode_t *);
  typedef const cypher_operator_t* (*op_getter)(const cypher_astnode_t *, unsigned int);

  void WalkQuery() const;
  void WalkStatement() const;
  void WalkStatementOption() const;
  void WalkCypherOption() const;
  void WalkCypherOptionParam() const;
  void WalkExplainOption() const;
  void WalkProfileOption() const;
  void WalkCreateNodePropIndex() const;
  void WalkDropNodePropIndex() const;
  void WalkCreateNodePropConstraint() const;
  void WalkDropNodePropConstraint() const;
  void WalkCreateRelPropConstraint() const;
  void WalkDropRelPropConstraint() const;
  void WalkUsingPeriodicCommit() const;
  void WalkLoadCsv() const;
  void WalkStart() const;
  void WalkNodeIndexLookup() const;
  void WalkNodeIndexQuery() const;
  void WalkNodeIdLookup() const;
  void WalkAllNodesScan() const;
  void WalkRelIndexLookup() const;
  void WalkRelIndexQuery() const;
  void WalkRelIdLookup() const;
  void WalkAllRelsScan() const;
  void WalkMatch() const;
  void WalkUsingIndex() const;
  void WalkUsingJoin() const;
  void WalkUsingScan() const;
  void WalkMerge() const;
  void WalkOnMatch() const;
  void WalkOnCreate() const;
  void WalkCreate() const;
  void WalkSet() const;
  void WalkSetProperty() const;
  void WalkSetAllProperties() const;
  void WalkMergeProperties() const;
  void WalkSetLabels() const;
  void WalkDelete() const;
  void WalkRemove() const;
  void WalkRemoveLabels() const;
  void WalkRemoveProperty() const;
  void WalkForEach() const;
  void WalkWith() const;
  void WalkUnwind() const;
  void WalkCall() const;
  void WalkReturn() const;
  void WalkProjection() const;
  void WalkOrderBy() const;
  void WalkSortItem() const;
  void WalkUnion() const;
  void WalkUnaryOperator() const;
  void WalkBinaryOperator() const;
  void WalkComparison() const;
  void WalkApplyOperator() const;
  void WalkApplyAllOperator() const;
  void WalkPropertyOperator() const;
  void WalkSubscriptOperator() const;
  void WalkSliceOperator() const;
  void WalkMapProjection() const;
  void WalkMapProjectionLiteral() const;
  void WalkMapProjectionProperty() const;
  void WalkMapProjectionIdentifier() const;
  void WalkMapProjectionAllProperties() const;
  void WalkLabelsOperator() const;
  void WalkListComprehension() const;
  void WalkPatternComprehension() const;
  void WalkCase() const;
  void WalkFilter() const;
  void WalkExtract() const;
  void WalkReduce() const;
  void WalkAll() const;
  void WalkAny() const;
  void WalkSingle() const;
  void WalkNone() const;
  void WalkCollection() const;
  void WalkMap() const;
  void WalkIdentifier() const;
  void WalkParameter() const;
  void WalkString() const;
  void WalkInteger() const;
  void WalkFloat() const;
  void WalkTrue() const;
  void WalkFalse() const;
  void WalkNull() const;
  void WalkLabel() const;
  void WalkRelType() const;
  void WalkPropName() const;
  void WalkFunctionName() const;
  void WalkIndexName() const;
  void WalkProcName() const;
  void WalkPattern() const;
  void WalkNamedPath() const;
  void WalkShortestPath() const;
  void WalkPatternPath() const;
  void WalkNodePattern() const;
  void WalkRelPattern() const;
  void WalkRange() const;
  void WalkCommand() const;
  void WalkLineComment() const;
  void WalkBlockComment() const;
  void WalkError() const;

  void AddMember(const char* key, const char* value) const;
  void AddMember(const char* key, int value) const;
  void AddMember(const char* key, bool value) const;
  void AddMember(const char* key, rapidjson::Value& value) const;
  void AddMemberInt(const char* key, specific_node_getter getter) const;
  void AddMemberInt(const char* key, const cypher_astnode_t* intNode) const;
  void AddMemberFloat(const char* key, specific_node_getter getter) const;
  void AddMemberFloat(const char* key, const cypher_astnode_t* floatNode) const;
  void AddMemberStr(const char* key, specific_node_getter getter) const;
  void AddMemberNull(const char* key) const;
  void AddMemberOp(const char* key, operator_getter getter) const;

  const char* ParseOp(const cypher_operator_t* op) const;
  void LoopNodes(const char* name, node_counter counter, node_getter getter) const;
  void LoopKeyValuePairs(const char* name, const char* keyName, const char* valueName,
                         node_counter counter, node_getter keyGetter, node_getter valueGetter) const;
  void LoopNodes(const char* name, unsigned int counter, node_getter getter) const;
  void LoopOps(const char* name, node_counter counter, op_getter getter) const;
  void Node(const char* name, const cypher_astnode_t* node) const;
  void Node(const char* name, specific_node_getter getter) const;
  void SwitchWalk(cypher_astnode_type_t nodeType) const;
  unsigned int LoopErrors(const cypher_parse_result_t* parseResult) const;
  void PrintAst(const cypher_parse_result_t* parseResult, unsigned int width,
                       const struct cypher_parser_colorization *colorization) const;

  const cypher_astnode_t *node;
  rapidjson::Value& parent;
  rapidjson::Document::AllocatorType& allocator;
};

#endif //__PARSER_HPP__