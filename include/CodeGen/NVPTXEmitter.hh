#ifndef INCLUDE_CODEGEN_NVPTXEMITTER_HH_
#define INCLUDE_CODEGEN_NVPTXEMITTER_HH_

#include <CodeGen/GenericEmitter.hh>
#include <AST/Api.hh>
#include <llvm/IR/IntrinsicsNVPTX.h>

namespace tlang::codegen {
struct NVPTXEmitter: public GenericEmitter {
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
    if (level == IdExpr::Matrix)
      return builder.CreateIntrinsic(si(false, expr->getCoordinate()), { }, { });
    return makeIntegerLiteral(ASTApi { ast_context }.CreateLiteral((int64_t) 0));
  }
  IRType_t<DimExpr> emitDimExpr(DimExpr *expr) {
    auto level = expr->getLevel();
    auto si = [](bool tid, DimExpr::Coordinate c) {
      switch (c) {
      case DimExpr::Y:
        return tid ? llvm::Intrinsic::nvvm_read_ptx_sreg_ntid_y : llvm::Intrinsic::nvvm_read_ptx_sreg_nctaid_y;
      case DimExpr::Z:
        return tid ? llvm::Intrinsic::nvvm_read_ptx_sreg_ntid_z : llvm::Intrinsic::nvvm_read_ptx_sreg_nctaid_z;
      default:
        return tid ? llvm::Intrinsic::nvvm_read_ptx_sreg_ntid_x : llvm::Intrinsic::nvvm_read_ptx_sreg_nctaid_x;
      }
    };
    if (level == DimExpr::Scalar || level == (DimExpr::Scalar | DimExpr::Matrix))
      return builder.CreateIntrinsic(si(true, expr->getCoordinate()), { }, { });
    if (level == DimExpr::Matrix)
      return builder.CreateIntrinsic(si(false, expr->getCoordinate()), { }, { });
    return makeIntegerLiteral(ASTApi { ast_context }.CreateLiteral((int64_t) 1));
  }
};
}

#endif
