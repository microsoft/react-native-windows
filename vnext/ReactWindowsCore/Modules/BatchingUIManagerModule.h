// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "UIManagerModule.h"

#include <folly/ProducerConsumerQueue.h>

#include <ViewManager.h>
#include <IUIManager.h>
#include <INativeUIManager.h>

#include <memory>
#include <vector>

namespace facebook { namespace react {

struct IReactRootView;
struct ShadowNode;

class BatchingUIManager
  : public UIManager
  , public ::std::enable_shared_from_this<BatchingUIManager>
{
public:
  BatchingUIManager(std::vector<std::unique_ptr<IViewManager>>&& viewManagers, INativeUIManager* nativeManager);
  virtual ~BatchingUIManager() noexcept {}

  void removeRootView(folly::dynamic&& args);
  void createView(folly::dynamic&& args);
  void setChildren(folly::dynamic&& args);
  void updateView(folly::dynamic&& args);
  void removeSubviewsFromContainerWithID(folly::dynamic&& args);
  void manageChildren(folly::dynamic&& args);
  void dispatchViewManagerCommand(folly::dynamic&& args);
  void replaceExistingNonRootView(folly::dynamic&& args);
  void measure(folly::dynamic&& args, facebook::xplat::module::CxxModule::Callback cb);
  void configureNextLayoutAnimation(folly::dynamic&& args, facebook::xplat::module::CxxModule::Callback success, facebook::xplat::module::CxxModule::Callback error);

  void onBatchComplete() override;

  void focus(folly::dynamic&& args);
  void blur(folly::dynamic&& args);
private:
  void dispatchFunction(std::function<void()> func);
  void processQueue();

private:
  std::shared_ptr<folly::ProducerConsumerQueue<std::function<void()>>> m_queue;
};

class BatchingUIManagerModule : public facebook::xplat::module::CxxModule
{
public:
  BatchingUIManagerModule(std::shared_ptr<IUIManager>&& manager);

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

private:
  std::shared_ptr<IUIManager> m_manager;
};

} }
