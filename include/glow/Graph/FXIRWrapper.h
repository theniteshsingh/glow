// (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

#ifndef GLOW_GRAPH_FXIRWRAPPER_H
#define GLOW_GRAPH_FXIRWRAPPER_H

#include "glow/Graph/Graph.h"

#include <folly/dynamic.h>

namespace glow {

using FXModule = folly::dynamic;
using FXNode = folly::dynamic;
using FXNodeList = folly::dynamic;
using FXWeight = folly::dynamic;

/// A thin wrapper around FX IR to provides common APIs. This is not mean to be
/// yet another IR, e.g., Glow high level graph IR.
class FXIRWrapper final : public IRContainer {
  /// Mapping from constant name to a void pointer points to where the constant
  /// is actually stored.
  const llvm::StringMap<const void *> &constants_;

  /// A reference to the owner of the function.
  Module *parent_;

  /// Mapping from getattrs to the underlying name of the constants they alias.
  llvm::StringMap<const std::string> getattrs_;

  /// A reference to the FXIR that this FXIRWrapper wraps.
  const FXModule *fx_mod_;

public:
  FXIRWrapper(const FXModule &FXIR, const std::string &submod,
              const llvm::StringMap<const void *> &constants,
              Module *glowModule, llvm::StringRef name = {})
      : IRContainer(name), constants_(constants), parent_(glowModule) {
    fx_mod_ = &(FXIR["modules"][submod]);
  }

  ~FXIRWrapper() = default;

  IRKind getIRKind() const override { return IRKind::GlowFXIRKind; };

  static bool classof(const IRContainer *I) {
    return I->getIRKind() == IRKind::GlowFXIRKind;
  }

  static bool classof(const FXIRWrapper *F) { return true; }

  /// \returns the underlying name of a Constant given provided \p name. If \p
  /// name is already the name of a Constant it is returned, else looks for
  /// getattr aliases to return the name of the actual underlying Constant.
  const char *getConstantName(llvm::StringRef name) const;

  /// \returns the underlying constant pointer given provided \p name. If \p
  /// name is already the name of a Constant, then the name is directly used,
  /// else looks for getattr aliases to find the name of the actual underlying
  /// Constant.
  const void *getConstant(llvm::StringRef name);

  /// \returns the weights of the graph.
  const FXWeight &getWeight();

  /// \returns the nodes of the graph.
  const FXNodeList &getNodes();

  /// \returns parent module that owns this graph.
  Module *getParent() override { return parent_; }
  const Module *getParent() const override { return parent_; }
};

} // namespace glow

#endif // GLOW_GRAPH_FXIRWRAPPER_H