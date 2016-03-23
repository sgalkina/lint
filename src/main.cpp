#include <string>
#include <stack>
#include <iostream>
#include <sstream>
#include <cassert>
#include <unordered_set>
#include "lint.h"

using namespace apa;

enum struct OpKind {ADD, SUB, MUL, DIV, REM, GT, GTE, LT, LTE, EQ, NEQ, POW, PUSH};

bool parse_operator(std::string const & op, OpKind & out) {
  if (op == "-") out = OpKind::SUB;
  else if (op == "+") out = OpKind::ADD;
  else if (op == "*") out = OpKind::MUL;
  else if (op == "/") out = OpKind::DIV;
  else if (op == "<") out = OpKind::LT;
  else if (op == "==") out = OpKind::EQ;
  else if (op == "!=") out = OpKind::NEQ;
  else if (op == "<=") out = OpKind::LTE;
  else if (op == ">") out = OpKind::GT;
  else if (op == ">=") out = OpKind::GTE;
  else if (op == "%%") out = OpKind::REM;
  else if (op == "pow") out = OpKind::POW;
  else return false;
  return true;
}

lint operations_mapping(OpKind op, const lint & lint1, const lint & lint2) {
  switch (op) {
    case OpKind::SUB: return lint1 - lint2;
    case OpKind::ADD: return lint1 + lint2;
    case OpKind::MUL: return lint1 * lint2;
    case OpKind::DIV: return lint1 / lint2;
    case OpKind::LT: return lint1 < lint2;
    case OpKind::EQ: return lint1 == lint2;
    case OpKind::NEQ: return lint1 != lint2;
    case OpKind::LTE: return lint1 <= lint2;
    case OpKind::GT: return lint1 > lint2;
    case OpKind::GTE: return lint1 >= lint2;
    case OpKind::REM: return lint1 % lint2;
    case OpKind::POW: return pow(lint1, std::stoll(lint2.to_string()));
    default: assert (false && "Not an operation");
  }
}

struct Operation {
  OpKind type;
  lint arg;
};

std::vector<Operation> parse_rpn(std::string const& expression) {
  std::string n;
  std::stringstream stream(expression);
  std::vector<Operation> operations;
  while (stream >> n) {
    OpKind op_type;
    Operation op;
    if (parse_operator(n, op_type)) {
      op.type = op_type;
    } else {
      lint arg(n);
      op.arg = arg;
      op.type = OpKind::PUSH;
    }
    operations.push_back(op);
  }
  return operations;
}

lint evaluate(std::vector<Operation> const& ops) {
  std::stack<lint> stack;
  for(auto const& op: ops) {
    if (op.type == OpKind::PUSH) {
      stack.push(op.arg);
    } else {
      lint l2 = stack.top();
      stack.pop();
      lint l1 = stack.top();
      stack.pop();
      stack.push(operations_mapping(op.type, l1, l2));
    }
  }
  return stack.top();
}

int main(int argc, char* argv[]) {
  std::string expression;
  size_t n = 100000;
  for (size_t i = 0; i < n; ++i) {
    if (std::getline(std::cin, expression)) {
      try {
        std::cout << evaluate(parse_rpn(expression)) << std::endl;
      } catch (int e) {
        if (e == 0) std::cout << "division by zero" << std::endl;
        else throw e;
      }
    }
  }
  return 0;
}
