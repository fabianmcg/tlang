#ifndef INCLUDE_CODEGEN_NVPTXEMITTER_HH_
#define INCLUDE_CODEGEN_NVPTXEMITTER_HH_

#include <CodeGen/GenericEmitter.hh>
#include <AST/Api.hh>
#include <llvm/IR/IntrinsicsNVPTX.h>

namespace tlang::codegen {
struct NVPTXEmitter: public GenericEmitter {
  static constexpr int warp_size = 32;
  using GenericEmitter::GenericEmitter;
  IRType_t<IdExpr> emitIdExpr(IdExpr *expr) {
    auto level = expr->getLevel();
    auto si = [](bool tid, IdExpr::Coordinate c) {
      switch (c) {
      case IdExpr::Y:
        return tid ? llvm::Intrinsic::nvvm_read_ptx_sreg_tid_y : llvm::Intrinsic::nvvm_read_ptx_sreg_ctaid_y;
      case IdExpr::Z:
        return tid ? llvm::Intrinsic::nvvm_read_ptx_sreg_tid_z : llvm::Intrinsic::nvvm_read_ptx_sreg_ctaid_z;
      default:
        return tid ? llvm::Intrinsic::nvvm_read_ptx_sreg_tid_x : llvm::Intrinsic::nvvm_read_ptx_sreg_ctaid_x;
      }
    };
    if (level == IdExpr::Scalar || level == (IdExpr::Scalar | IdExpr::Matrix))
      return builder.CreateIntrinsic(si(true, expr->getCoordinate()), { }, { });
    else if (level == IdExpr::Matrix)
      return builder.CreateIntrinsic(si(false, expr->getCoordinate()), { }, { });
    else if (level == (IdExpr::Scalar | IdExpr::Vector)) {
      llvm::Value *tid = builder.CreateIntrinsic(si(true, expr->getCoordinate()), { }, { });
      return builder.CreateURem(tid, makeInt32(warp_size, false));
    } else if (level == (IdExpr::Vector | IdExpr::Matrix)) {
      llvm::Value *tid = builder.CreateIntrinsic(si(true, expr->getCoordinate()), { }, { });
      return builder.CreateUDiv(tid, makeInt32(warp_size, false));
    }
    return makeIntegerLiteral(ASTApi { ast_context }.CreateLiteral((int64_t) 0));
  }
  IRType_t<DimExpr> emitDimExpr(DimExpr *expr) {
    auto level = expr->getLevel();
    auto si = [](bool block, DimExpr::Coordinate c) {
      switch (c) {
      case DimExpr::Y:
        return block ? llvm::Intrinsic::nvvm_read_ptx_sreg_ntid_y : llvm::Intrinsic::nvvm_read_ptx_sreg_nctaid_y;
      case DimExpr::Z:
        return block ? llvm::Intrinsic::nvvm_read_ptx_sreg_ntid_z : llvm::Intrinsic::nvvm_read_ptx_sreg_nctaid_z;
      default:
        return block ? llvm::Intrinsic::nvvm_read_ptx_sreg_ntid_x : llvm::Intrinsic::nvvm_read_ptx_sreg_nctaid_x;
      }
    };
    if (level == DimExpr::Vector)
      return makeInt32(warp_size, false);
    if (level == DimExpr::Matrix)
      return builder.CreateIntrinsic(si(true, expr->getCoordinate()), { }, { });
    if (level == DimExpr::Tensor)
      return builder.CreateIntrinsic(si(false, expr->getCoordinate()), { }, { });
    if (level == (DimExpr::Scalar | DimExpr::Matrix))
      return builder.CreateUDiv(builder.CreateIntrinsic(si(true, expr->getCoordinate()), { }, { }), makeInt32(warp_size, false));
    return makeInt32(1, false);
  }
  llvm::Value* makeBasicShfl(llvm::Value *mask, llvm::Value *value, llvm::Value *lane) {
    auto type = value->getType();
    std::vector<llvm::Type*> types = { llvm::IntegerType::getInt32Ty(context), value->getType(), llvm::IntegerType::getInt32Ty(context),
        llvm::IntegerType::getInt32Ty(context) };
    std::vector<llvm::Value*> args;
    llvm::Value *ci32 = llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(context), 31);
    args.push_back(mask);
    args.push_back(value);
    args.push_back(lane);
    args.push_back(ci32);
    if (type->isIntegerTy(32))
      return builder.CreateIntrinsic(llvm::Intrinsic::nvvm_shfl_sync_down_i32, { }, args);
    else if (type->isFloatTy())
      return builder.CreateIntrinsic(llvm::Intrinsic::nvvm_shfl_sync_down_f32, { }, args);
    assert(false);
    return nullptr;
  }
  llvm::Value* makeShfl(llvm::Value *mask, llvm::Value *value, llvm::Value *lane, Type *srcType) {
    auto type = value->getType();
    if (type->isIntegerTy(32) || type->isFloatTy())
      return makeBasicShfl(mask, value, lane);
    auto i32t = llvm::Type::getInt32Ty(context);
    auto i64t = llvm::Type::getInt64Ty(context);
    if (auto ft = dyn_cast<FloatType>(srcType)) {
      if (ft->isP_64()) {
        value = builder.CreateBitCast(value, i64t);
        auto lo = builder.CreateTrunc(value, i32t);
        value = builder.CreateLShr(value, 32);
        auto hi = builder.CreateTrunc(value, i32t);
        lo = builder.CreateZExt(makeBasicShfl(mask, lo, lane), i64t);
        hi = builder.CreateZExt(makeBasicShfl(mask, hi, lane), i64t);
        hi = builder.CreateShl(hi, 32, "", true);
        value = builder.CreateOr(hi, lo);
        return builder.CreateBitCast(value, llvm::Type::getDoubleTy(context));
      }
    }
    assert(false);
    return nullptr;
  }
  IRType_t<ReduceExpr> emitReduceExpr(ReduceExpr *expr) {
    auto type = expr->getExpr()->getType().getType();
    llvm::Value *value = emitExpr(expr->getExpr());
    llvm::Value *mask = llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(context), -1);
    value = makeAddOp(QualType(type), makeShfl(mask, value, makeInt32(16), type), value);
    value = makeAddOp(QualType(type), makeShfl(mask, value, makeInt32(8), type), value);
    value = makeAddOp(QualType(type), makeShfl(mask, value, makeInt32(4), type), value);
    value = makeAddOp(QualType(type), makeShfl(mask, value, makeInt32(2), type), value);
    value = makeAddOp(QualType(type), makeShfl(mask, value, makeInt32(1), type), value);
    return value;
  }
  virtual IRType_t<SyncStmt> emitSyncStmt(SyncStmt *stmt) {
    return builder.CreateIntrinsic(llvm::Intrinsic::nvvm_barrier0, { }, { });
  }
};
}

#endif
