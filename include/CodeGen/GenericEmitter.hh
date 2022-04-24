#ifndef CODEGEN_GENERICEMITTER_HH
#define CODEGEN_GENERICEMITTER_HH

#include <CodeGen/Emitter.hh>
#include <CodeGen/EmitterVisitor.hh>

namespace tlang::codegen {
class GenericEmitter: public impl::EmitterVisitor<GenericEmitter>, public Emitter {
public:
  using Emitter::Emitter;
  /******************************************************************************
   * Emit types
   ******************************************************************************/
  llvm::Type* makeVoid();
  llvm::PointerType* makeAddress(int adressSpace = 0);
  llvm::PointerType* makePointer(llvm::Type *type, int adressSpace = 0);
  IRType_t<QualType> emitQualType(QualType type);
  IRType_t<AddressType> emitAddressType(AddressType *type, int as = 0);
  IRType_t<BoolType> emitBoolType(BoolType *type, int as = 0);
  IRType_t<IntType> emitIntType(IntType *type, int as = 0);
  IRType_t<FloatType> emitFloatType(FloatType *type, int as = 0);
  IRType_t<PtrType> emitPtrType(PtrType *type, int as = 0);
  IRType_t<ArrayType> emitArrayType(ArrayType *type, int as = 0);
  IRType_t<StructType> emitStructType(StructType *type, int as = 0);
  IRType_t<FunctionType> emitFunctionType(FunctionType *type, int as = 0);
  /******************************************************************************
   * Emit declarations
   ******************************************************************************/
  llvm::AllocaInst* makeVariable(VariableDecl *variable, const std::string &suffix = "");
  IRType_t<FunctionType> makeFunctionType(FunctorDecl *functor);
  IRType_t<FunctorDecl> makeFunction(FunctorDecl *functor);
  IRType_t<ModuleDecl> emitModuleDecl(ModuleDecl *module);
  IRType_t<FunctionDecl> emitFunctionDecl(FunctionDecl *function);
  IRType_t<VariableDecl> emitVariableDecl(VariableDecl *variable);
  IRType_t<ParameterDecl> emitParameterDecl(ParameterDecl *variable);
  /******************************************************************************
   * Emit statements
   ******************************************************************************/
  inline llvm::Value* emitBranch(llvm::BasicBlock *current_block, llvm::BasicBlock *to_block) {
    if (current_block && !current_block->getTerminator())
      return builder.CreateBr(to_block);
    return nullptr;
  }
  inline llvm::Value* emitBranch(llvm::BasicBlock *to_block) {
    return emitBranch(builder.GetInsertBlock(), to_block);
  }
  inline std::string makeLabel(ASTKind kind, const std::string label) {
    return label + "." + std::to_string(counters[kind]);
  }
  inline void incrementLabel(ASTKind kind) {
    counters[kind]++;
  }
  void resetCounters() {
    counters.clear();
  }

  IRType_t<IfStmt> emitIfStmt(IfStmt *stmt);
  IRType_t<ForStmt> emitForStmt(ForStmt *stmt);
  IRType_t<WhileStmt> emitWhileStmt(WhileStmt *stmt);
  IRType_t<BreakStmt> emitBreakStmt(BreakStmt *stmt);
  IRType_t<ContinueStmt> emitContinueStmt(ContinueStmt *stmt);
  IRType_t<ReturnStmt> emitReturnStmt(ReturnStmt *stmt);
  IRType_t<SyncStmt> emitSyncStmt(SyncStmt *stmt);
  /******************************************************************************
   * Emit expressions
   ******************************************************************************/
  llvm::Value* makeAddOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeSubOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeMulOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeDivOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeLogic(BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeCmp(QualType type, BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeLoad(QualType type, llvm::Value *value);
  llvm::Value* makeStore(llvm::Value *value, llvm::Value *ptr);
  IRType_t<BooleanLiteral> makeBooleanLiteral(BooleanLiteral *literal);
  IRType_t<IntegerLiteral> makeIntegerLiteral(IntegerLiteral *literal);
  IRType_t<UIntegerLiteral> makeIntegerLiteral(UIntegerLiteral *literal);
  IRType_t<FloatLiteral> makeFloatLiteral(FloatLiteral *literal);
  IRType_t<CastExpr> makeCast(Type *dest, Type *source, llvm::Value *subExpr);
  IRType_t<BooleanLiteral> emitBooleanLiteral(BooleanLiteral *literal);
  IRType_t<IntegerLiteral> emitIntegerLiteral(IntegerLiteral *literal);
  IRType_t<UIntegerLiteral> emitUIntegerLiteral(UIntegerLiteral *literal);
  IRType_t<FloatLiteral> emitFloatLiteral(FloatLiteral *literal);
  IRType_t<ParenExpr> emitParenExpr(ParenExpr *expr);
  IRType_t<UnaryOperator> emitUnaryOperator(UnaryOperator *expr);
  IRType_t<BinaryOperator> emitBinaryOperator(BinaryOperator *expr);
  IRType_t<DeclRefExpr> emitDeclRefExpr(DeclRefExpr *expr);
  IRType_t<MemberExpr> emitMemberExpr(MemberExpr *expr);
  IRType_t<CallExpr> emitCallExpr(CallExpr *expr);
  //  IRType_t<MemberCallExpr> emitMemberCallExpr(MemberCallExpr *expr);
  IRType_t<ArrayExpr> emitArrayExpr(ArrayExpr *expr);
  IRType_t<CastExpr> emitCastExpr(CastExpr *expr);
  IRType_t<ImplicitCastExpr> emitImplicitCastExpr(ImplicitCastExpr *expr);
  IRType_t<TernaryOperator> emitTernaryOperator(TernaryOperator *expr);
  IRType_t<IdExpr> emitIdExpr(IdExpr *expr);
protected:
  std::unordered_map<Decl*, llvm::Type*> decl2type { };
  std::map<ASTKind, int> counters;
  llvm::BasicBlock *end_block { };
};
}

#endif
