// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "BatchingUIManagerModule.h"

#include <algorithm>
#include <cxxreact/JsArgumentHelpers.h>
#include <folly/json.h>

using namespace folly;
using namespace facebook::xplat;
using namespace std;

namespace facebook {
namespace react {

BatchingUIManager::BatchingUIManager(std::vector<std::unique_ptr<IViewManager>>&& viewManagers, INativeUIManager* nativeManager)
  : UIManager(std::move(viewManagers), nativeManager)
{
  m_queue = std::make_unique<folly::ProducerConsumerQueue<std::function<void()>>>(2048);
}

#define TRACK_QUEUE
#ifdef TRACK_QUEUE
  static uint32_t cBatches = 0;
  static uint32_t cCalls = 0;
#endif

void BatchingUIManager::processQueue()
{
#ifdef TRACK_QUEUE
  uint32_t cTheseCalls = 0;
#endif

  std::function<void()> func;
  while (m_queue->read(func)) {
    func();

#ifdef TRACK_QUEUE
    ++cTheseCalls;
#endif
  }

#ifdef TRACK_QUEUE
  cCalls += cTheseCalls;
  char buffer[1024];
  _snprintf_s(buffer, _countof(buffer), _TRUNCATE, "BatchingUIManager Batches: %u Calls: %u (%u new)\r\n", ++cBatches, cCalls, cTheseCalls);
  OutputDebugStringA(buffer);
#endif
}

void BatchingUIManager::dispatchFunction(std::function<void()> func)
{
  // If the queue is full then dispatch all items in the queue first
  if (this->m_queue->isFull())
  {
#ifdef TRACK_QUEUE
  OutputDebugStringA("BatchingUIManager: Processing Full Queue ...");
#endif

    processQueue();
  }

  m_queue->write(func);
}

void BatchingUIManager::onBatchComplete()
{
  processQueue();
  UIManager::onBatchComplete();
}

void BatchingUIManager::removeRootView(folly::dynamic&& args)
{
  auto manager = std::static_pointer_cast<UIManager>(shared_from_this());
  dispatchFunction([manager, args]()
  {
    manager->removeRootView(jsArgAsInt(args, 0));
  });
}

void BatchingUIManager::configureNextLayoutAnimation(folly::dynamic&& args, facebook::xplat::module::CxxModule::Callback success, facebook::xplat::module::CxxModule::Callback error)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args, success, error]()
  {
    auto config = jsArgAsDynamic(args, 3);
    manager->configureNextLayoutAnimation(std::move(config), success, error);
  });
}

void BatchingUIManager::createView(folly::dynamic&& args)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args]()
  {
    auto arg3 = jsArgAsDynamic(args, 3);
    static_cast<IUIManager*>(manager.get())->createView(jsArgAsInt(args, 0), jsArgAsString(args, 1), jsArgAsInt(args, 2), std::move(arg3));
  });
}

void BatchingUIManager::setChildren(folly::dynamic&& args)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args]()
  {
    auto arg1 = jsArgAsArray(args, 1);
    static_cast<IUIManager*>(manager.get())->setChildren(jsArgAsInt(args, 0), std::move(arg1));
  });
}

void BatchingUIManager::updateView(folly::dynamic&& args)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args]()
  {
    auto arg2 = jsArgAsDynamic(args, 2);
    static_cast<IUIManager*>(manager.get())->updateView(jsArgAsInt(args, 0), jsArgAsString(args, 1), std::move(arg2));
  });
}

void BatchingUIManager::removeSubviewsFromContainerWithID(folly::dynamic&& args)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args]()
  {
    static_cast<IUIManager*>(manager.get())->removeSubviewsFromContainerWithID(jsArgAsInt(args, 0));
  });
}

void BatchingUIManager::manageChildren(folly::dynamic&& args)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args]()
  {
    auto arg1 = jsArgAsDynamic(args, 1);
    auto arg2 = jsArgAsDynamic(args, 2);
    auto arg3 = jsArgAsDynamic(args, 3);
    auto arg4 = jsArgAsDynamic(args, 4);
    auto arg5 = jsArgAsDynamic(args, 5);
    static_cast<IUIManager*>(manager.get())->manageChildren(jsArgAsInt(args, 0), arg1, arg2, arg3, arg4, arg5);
  });
}

void BatchingUIManager::dispatchViewManagerCommand(folly::dynamic&& args)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args]()
  {
    auto arg2 = jsArgAsDynamic(args, 2);
    static_cast<IUIManager*>(manager.get())->dispatchViewManagerCommand(jsArgAsInt(args, 0), jsArgAsInt(args, 1), std::move(arg2));
  });
}

void BatchingUIManager::replaceExistingNonRootView(folly::dynamic&& args)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args]()
  {
    static_cast<IUIManager*>(manager.get())->replaceExistingNonRootView(jsArgAsInt(args, 0), jsArgAsInt(args, 1));
  });
}

void BatchingUIManager::measure(folly::dynamic&& args, facebook::xplat::module::CxxModule::Callback cb)
{
  auto manager = shared_from_this();
  dispatchFunction([manager, args, cb]()
  {
    static_cast<IUIManager*>(manager.get())->measure(jsArgAsInt(args, 0), cb);
  });
}


BatchingUIManagerModule::BatchingUIManagerModule(std::shared_ptr<IUIManager>&& manager)
	: m_manager(std::move(manager))
{
}

std::string BatchingUIManagerModule::getName()
{
	return "UIManager";
}

std::map<std::string, folly::dynamic> BatchingUIManagerModule::getConstants()
{
	std::map<std::string, folly::dynamic> constants {};

	m_manager->populateViewManagerConstants(constants);

	return constants;
}

std::vector<facebook::xplat::module::CxxModule::Method> BatchingUIManagerModule::getMethods()
{
  auto manager = std::static_pointer_cast<BatchingUIManager>(m_manager);
  return
  {
    Method("getConstantsForViewManager", [manager](dynamic argsJson) -> dynamic
    {
      dynamic args;
      if (argsJson.isString())
      {
        args = folly::parseJson(argsJson.asString());
      }
      else
      {
        // In JSI mode this is an array
        assert(argsJson.isArray());
        args = argsJson;
      }
      return manager->getConstantsForViewManager(jsArgAsString(args, 0));
   }, SyncTag),
    Method("removeRootView", [manager](dynamic args)
    {
      manager->removeRootView(std::move(args));
    }),
    Method("createView", [manager](dynamic args)
    {
      manager->createView(std::move(args));
    }),
    Method("configureNextLayoutAnimation", [manager](dynamic args, Callback cbSuccess, Callback cbError)
    {
      manager->configureNextLayoutAnimation(std::move(args), cbSuccess, cbError);
    }, AsyncTag),
    Method("setChildren", [manager](dynamic args)
    {
      manager->setChildren(std::move(args));
    }),
    Method("updateView", [manager](dynamic args)
    {
      manager->updateView(std::move(args));
    }),
    Method("removeSubviewsFromContainerWithID", [manager](dynamic args)
    {
      manager->removeSubviewsFromContainerWithID(std::move(args));
    }),
    Method("manageChildren", [manager](dynamic args)
    {
      manager->manageChildren(std::move(args));
    }),
    Method("replaceExistingNonRootView", [manager](dynamic args)
    {
      manager->replaceExistingNonRootView(std::move(args));
    }),
    Method("dispatchViewManagerCommand", [manager](dynamic args)
    {
      manager->dispatchViewManagerCommand(std::move(args));
    }),
    Method("measure", [manager](dynamic args, facebook::xplat::module::CxxModule::Callback cb)
    {
      manager->measure(std::move(args), cb);
    }),
    Method("setJSResponder", [](dynamic args)
    {
      // TODO: Implement?
      //manager->setJSResponder(std::move(args));
    }),
    Method("clearJSResponder", []()
    {
      // TODO: Implement?
      //manager->clearJSResponder();
    }),
  };
}

shared_ptr<IUIManager> createBatchingUIManager(std::vector<std::unique_ptr<IViewManager>>&& viewManagers, INativeUIManager* nativeManager)
{
	return std::make_shared<BatchingUIManager>(std::move(viewManagers), nativeManager);
}

std::unique_ptr<facebook::xplat::module::CxxModule> createBatchingUIManagerModule(std::shared_ptr<IUIManager> uimanager)
{
	return std::make_unique<BatchingUIManagerModule>(std::move(uimanager));
}

}
} // namespace facebook::react
