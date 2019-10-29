// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>

#include <folly/dynamic.h>
#include <stdint.h>
#include <string>

namespace facebook {
namespace react {

struct INativeUIManager;
struct IReactRootView;
class IViewManager;
struct ShadowNode;

class IUIManager {
 public:
  virtual ~IUIManager(){};
  virtual int64_t AddMeasuredRootView(IReactRootView *rootView) = 0;

  // TODO: Remove if moduleRegistry exposes onBatchComplete
  virtual void onBatchComplete() = 0;

  virtual folly::dynamic getConstantsForViewManager(const std::string &viewManager) = 0;
  virtual void populateViewManagerConstants(std::map<std::string, folly::dynamic> &constants) = 0;
  virtual void createView(int64_t tag, std::string &&className, int64_t rootViewTag, folly::dynamic &&props) = 0;
  virtual void configureNextLayoutAnimation(
      folly::dynamic &&config,
      facebook::xplat::module::CxxModule::Callback success,
      facebook::xplat::module::CxxModule::Callback error) = 0;
  virtual void removeRootView(int64_t rootViewTag) = 0;
  virtual void setChildren(int64_t viewTag, folly::dynamic &&childrenTags) = 0;
  virtual void updateView(int64_t tag, const std::string &className, folly::dynamic &&props) = 0;
  virtual void removeSubviewsFromContainerWithID(int64_t containerTag) = 0;
  virtual void manageChildren(
      int64_t viewTag,
      folly::dynamic &moveFrom,
      folly::dynamic &moveTo,
      folly::dynamic &addChildTags,
      folly::dynamic &addAtIndices,
      folly::dynamic &removeFrom) = 0;
  virtual void dispatchViewManagerCommand(int64_t reactTag, int64_t commandId, folly::dynamic &&commandArgs) = 0;
  virtual void replaceExistingNonRootView(int64_t oldTag, int64_t newTag) = 0;
  virtual void measure(int64_t reactTag, facebook::xplat::module::CxxModule::Callback callback) = 0;
  virtual void measureInWindow(int64_t reactTag, facebook::xplat::module::CxxModule::Callback callback) = 0;
  virtual INativeUIManager *getNativeUIManager() = 0;

  virtual void focus(int64_t tag) = 0;
  virtual void blur(int64_t tag) = 0;

  // needed for control specific native modules to get the shadow node from the
  // UIManager given its tag
  virtual ShadowNode *FindShadowNodeForTag(int64_t tag) = 0;
  virtual void findSubviewIn(
      int64_t reactTag,
      folly::dynamic &&coordinates,
      facebook::xplat::module::CxxModule::Callback callback) = 0;
};

std::shared_ptr<IUIManager> createIUIManager(
    std::vector<std::unique_ptr<IViewManager>> &&viewManagers,
    INativeUIManager *nativeManager);
std::unique_ptr<facebook::xplat::module::CxxModule> createUIManagerModule(std::shared_ptr<IUIManager> uimanager);

std::shared_ptr<IUIManager> createBatchingUIManager(
    std::vector<std::unique_ptr<IViewManager>> &&viewManagers,
    INativeUIManager *nativeManager);
std::unique_ptr<facebook::xplat::module::CxxModule> createBatchingUIManagerModule(
    std::shared_ptr<IUIManager> uimanager);

} // namespace react
} // namespace facebook
