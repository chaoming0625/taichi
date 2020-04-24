#include "taichi/ir/ir.h"
#include <unordered_set>

TLANG_NAMESPACE_BEGIN

class UsedAtomicsSearcher : public BasicStmtVisitor {
 private:
  std::unordered_set<AtomicOpStmt *> used_atomics;

 public:
  UsedAtomicsSearcher() {
    allow_undefined_visitor = true;
    invoke_default_visitor = true;
  }

  void search_operands(Stmt *stmt) {
    for (auto &op : stmt->get_operands()) {
      if (op != nullptr && op->is<AtomicOpStmt>()) {
        used_atomics.insert(op->as<AtomicOpStmt>());
      }
    }
  }

  void preprocess_container_stmt(Stmt *stmt) override {
    search_operands(stmt);
  }

  void visit(Stmt *stmt) override {
    search_operands(stmt);
  }

  static std::unordered_set<AtomicOpStmt *> run(IRNode *root) {
    UsedAtomicsSearcher searcher;
    root->accept(&searcher);
    return searcher.used_atomics;
  }
};

namespace irpass::analysis {
std::unordered_set<AtomicOpStmt *> gather_used_atomics(IRNode *root) {
  return UsedAtomicsSearcher::run(root);
}
}  // namespace irpass::analysis

TLANG_NAMESPACE_END
