/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
#pragma once
#include "Yoga-internal.h"
#include "Yoga.h"

struct YGConfig {
  using LogWithContextFn = int (__cdecl *)(
      YGConfigRef config,
      YGNodeRef node,
      YGLogLevel level,
      void* context,
      const char* format,
      va_list args);
  using CloneWithContextFn = YGNodeRef (__cdecl *)(
      YGNodeRef node,
      YGNodeRef owner,
      int childIndex,
      void* cloneContext);

private:
  union {
    CloneWithContextFn withContext;
    YGCloneNodeFunc noContext;
  } cloneNodeCallback_;
  union {
    LogWithContextFn withContext;
    YGLogger noContext;
  } logger_;
  bool cloneNodeUsesContext_;
  bool loggerUsesContext_;

public:
  bool useWebDefaults = false;
  bool useLegacyStretchBehaviour = false;
  bool shouldDiffLayoutWithoutLegacyStretchBehaviour = false;
  bool printTree = false;
  float pointScaleFactor = 1.0f;
  std::array<bool, facebook::yoga::enums::count<YGExperimentalFeature>()>
      experimentalFeatures = {};
  void* context = nullptr;

  YGConfig(YGLogger logger);
  void __cdecl log(YGConfig*, YGNode*, YGLogLevel, void*, const char*, va_list);
  void __cdecl setLogger(YGLogger logger) {
    logger_.noContext = logger;
    loggerUsesContext_ = false;
  }
  void __cdecl setLogger(LogWithContextFn logger) {
    logger_.withContext = logger;
    loggerUsesContext_ = true;
  }
  void __cdecl setLogger(std::nullptr_t) { setLogger(YGLogger{nullptr}); }

  YGNodeRef __cdecl cloneNode(
      YGNodeRef node,
      YGNodeRef owner,
      int childIndex,
      void* cloneContext);
  void __cdecl setCloneNodeCallback(YGCloneNodeFunc cloneNode) {
    cloneNodeCallback_.noContext = cloneNode;
    cloneNodeUsesContext_ = false;
  }
  void __cdecl setCloneNodeCallback(CloneWithContextFn cloneNode) {
    cloneNodeCallback_.withContext = cloneNode;
    cloneNodeUsesContext_ = true;
  }
  void __cdecl setCloneNodeCallback(std::nullptr_t) {
    setCloneNodeCallback(YGCloneNodeFunc{nullptr});
  }
};
