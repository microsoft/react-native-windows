// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using namespace std;
#include <iostream>
#include <algorithm>
#include <cxxreact/JsArgumentHelpers.h>
#include <folly/json.h>

#include "UIManagerModule.h"
#include "ShadowNode.h"
#include "ShadowNodeRegistry.h"

#include <IReactRootView.h>

using namespace folly;
using namespace facebook::xplat;

namespace facebook {
namespace react {

UIManager::UIManager(std::vector<std::unique_ptr<IViewManager>>&& viewManagers, INativeUIManager* nativeManager)
	: m_viewManagers(std::move(viewManagers))
	, m_nativeUIManager(nativeManager)
{
	m_nativeUIManager->setHost(this);
}

UIManager::~UIManager()
{
	m_nodeRegistry.removeAllRootViews([this](int64_t rootViewTag)
	{
		removeRootView(rootViewTag);
	});

	m_nativeUIManager->setHost(nullptr);
	m_nativeUIManager->destroy();
}

folly::dynamic UIManager::getConstantsForViewManager(const std::string& className)
{
	const IViewManager* vm = GetViewManager(className);
  if (vm != nullptr)
			return vm->GetConstants();
	return nullptr;
}

void UIManager::populateViewManagerConstants(std::map<std::string, dynamic>& constants)
{
	for (auto&& vm : m_viewManagers)
		constants.emplace(vm->GetName(), vm->GetConstants());
}

IViewManager* UIManager::GetViewManager(const std::string& className) const
{
	for (auto&& vm : m_viewManagers)
	{
		if (!strcmp(vm->GetName(), className.c_str()))
			return vm.get();
	}
	return nullptr;
}

void UIManager::RegisterRootView(IReactRootView* rootView, int64_t rootViewTag, int64_t width, int64_t height)
{
  static std::string rootClassName = "ROOT";
  auto viewManager = GetViewManager(rootClassName);

  auto root = m_nativeUIManager->createRootShadowNode(rootView);
	root->m_className = rootClassName;
  root->m_viewManager = viewManager;
	root->m_tag = rootViewTag;
	m_nodeRegistry.addRootView(shadow_ptr(root), rootViewTag);

  m_nativeUIManager->AddRootView(*root, rootView);
}

void UIManager::removeRootView(int64_t rootViewTag)
{
  m_nativeUIManager->ensureInBatch();
  auto& node = m_nodeRegistry.getRoot(rootViewTag);
	m_nativeUIManager->removeRootView(node);
	DropView(rootViewTag, true);
	m_nodeRegistry.removeRootView(rootViewTag);
	m_nativeUIManager->destroyRootShadowNode(&node);
}

void UIManager::zombieView(int64_t tag)
{
	DropView(tag, false, true);
}

void UIManager::DropView(int64_t tag, bool removeChildren /*= true*/, bool zombieView /* = false */)
{
	auto& node = m_nodeRegistry.getNode(tag);

	node.onDropViewInstance();

  m_nativeUIManager->RemoveView(node, removeChildren);

	if (zombieView)
		node.m_zombie = true;

	for (auto childTag : node.m_children)
		DropView(childTag, removeChildren, zombieView);

	if (removeChildren)
		node.removeAllChildren();

	if (!zombieView)
		m_nodeRegistry.removeNode(tag);
}

void UIManager::removeSubviewsFromContainerWithID(int64_t containerTag)
{
  m_nativeUIManager->ensureInBatch();
  auto& containerNode = m_nodeRegistry.getNode(containerTag);

	std::vector<int64_t> indicesToRemove(containerNode.m_children.size());
	for (size_t i = 0; i < containerNode.m_children.size(); i++)
		indicesToRemove[static_cast<size_t>(i)] = static_cast<int64_t>(i);
	std::vector<int64_t> emptyVec;
	manageChildren(containerTag, emptyVec, emptyVec, emptyVec, emptyVec, indicesToRemove);
}

std::vector<int64_t> PopulateIntVecFromDynamicArray(const folly::dynamic& arr)
{
	std::vector<int64_t> vec(arr.empty() ? 0 : arr.size());

	if (arr.empty())
		return vec;

	auto i = 0;
	for (auto& val : arr)
	{
		vec[i] = val.asInt();
		++i;
	}
	return vec;
}

void UIManager::manageChildren(int64_t viewTag,
	folly::dynamic& moveFrom,
	folly::dynamic& moveTo,
	folly::dynamic& addChildTags,
	folly::dynamic& addAtIndices,
	folly::dynamic& removeFrom)
{
	auto vecMoveFrom = PopulateIntVecFromDynamicArray(moveFrom);
	auto vecMoveTo = PopulateIntVecFromDynamicArray(moveTo);
	auto vecAddChildTags = PopulateIntVecFromDynamicArray(addChildTags);
	auto vecAddAtIndices = PopulateIntVecFromDynamicArray(addAtIndices);
	auto vecRemoveFrom = PopulateIntVecFromDynamicArray(removeFrom);
	manageChildren(viewTag, vecMoveFrom, vecMoveTo, vecAddChildTags, vecAddAtIndices, vecRemoveFrom);
}

struct ViewAtIndex
{
	int64_t tag;
	int64_t index;
	static bool ViewAtIndexCompare(const std::shared_ptr<ViewAtIndex>& x, const std::shared_ptr<ViewAtIndex>& y) noexcept
	{
		return (x->index < y->index);
	}
};


void UIManager::manageChildren(int64_t viewTag,
	std::vector<int64_t>& moveFrom,
	std::vector<int64_t>& moveTo,
	std::vector<int64_t>& addChildTags,
	std::vector<int64_t>& addAtIndices,
	std::vector<int64_t>& removeFrom)
{
  m_nativeUIManager->ensureInBatch();
  auto& shadowNodeToManage = m_nodeRegistry.getNode(viewTag);

	auto numToMove = moveFrom.size();
	auto numToAdd = addChildTags.size();
	auto numToRemove = removeFrom.size();
	std::vector<shared_ptr<ViewAtIndex>> viewsToAdd(numToMove + numToAdd);
  std::vector<shared_ptr<ViewAtIndex>> viewsToRemove(numToMove + numToRemove);
	std::vector<int64_t> tagsToDelete(numToRemove);

	if (numToMove > 0)
	{
		for (size_t i = 0; i < numToMove; ++i)
		{
			auto moveFromIndex = moveFrom[i];
			auto tagToMove = m_nodeRegistry.getNode(static_cast<size_t>(shadowNodeToManage.m_children[static_cast<size_t>(moveFromIndex)])).m_tag;
			viewsToAdd[i] = make_shared<ViewAtIndex>(ViewAtIndex { tagToMove, moveTo[i] });
      viewsToRemove[i] = make_shared<ViewAtIndex>(ViewAtIndex{ tagToMove, moveFromIndex });
		}
	}

	if (numToAdd > 0)
	{
		for (size_t i = 0; i < numToAdd; ++i)
		{
			viewsToAdd[numToMove + i] = make_shared<ViewAtIndex>(ViewAtIndex { addChildTags[i], addAtIndices[i] });
		}
	}

	if (numToRemove > 0)
	{
		for (size_t i = 0; i < numToRemove; ++i)
		{
			auto indexToRemove = removeFrom[i];
			auto tagToRemove = m_nodeRegistry.getNode(static_cast<size_t>(shadowNodeToManage.m_children[static_cast<size_t>(indexToRemove)])).m_tag;
      viewsToRemove[i] = make_shared<ViewAtIndex>(ViewAtIndex{ tagToRemove, indexToRemove });
			tagsToDelete[i] = tagToRemove;
		}
	}

	// NB: moveFrom and removeForm are both relative to the starting
	// state of the view's children.
	//
	// 1) Sort the views to add and indices to remove by index
	// 2) Iterate the indices being removed from high to low and remove
	//    them. Going high to low makes sure we remove the correct
	//    index when there are multiple to remove.
	// 3) Iterate the views being added by index low to high and add
	//    them. Like the view removal, iteration direction is important
	//    to preserve the correct index.

	std::sort(viewsToAdd.begin(), viewsToAdd.end(), ViewAtIndex::ViewAtIndexCompare);
  std::sort(viewsToRemove.begin(), viewsToRemove.end(), ViewAtIndex::ViewAtIndexCompare);

	// Apply changes to the ReactShadowNode hierarchy.
	for (auto i = static_cast<int>(viewsToRemove.size()) - 1; i >= 0; --i)
	{
    auto viewAtIndex = viewsToRemove[i];
    auto& shadowNodeToRemove = m_nodeRegistry.getNode(viewAtIndex->tag);

    shadowNodeToManage.m_children.erase(shadowNodeToManage.m_children.begin() + static_cast<size_t>(viewAtIndex->index));
		shadowNodeToManage.RemoveChildAt(viewAtIndex->index);
	}

	for (size_t i = 0; i < viewsToAdd.size(); ++i)
	{
		auto viewAtIndex = viewsToAdd[i];
		auto& shadowNodeToAdd = m_nodeRegistry.getNode(viewAtIndex->tag);
		shadowNodeToAdd.m_parent = shadowNodeToManage.m_tag;
		shadowNodeToManage.m_children.insert(shadowNodeToManage.m_children.begin() + static_cast<size_t>(viewAtIndex->index), shadowNodeToAdd.m_tag);
    if (!shadowNodeToManage.m_zombie)
      shadowNodeToManage.AddView(shadowNodeToAdd, viewAtIndex->index);

    m_nativeUIManager->AddView(shadowNodeToManage, shadowNodeToAdd, viewAtIndex->index);
	}

	for (auto tagToDelete : tagsToDelete)
		DropView(tagToDelete);
}

void UIManager::configureNextLayoutAnimation(folly::dynamic&& config, facebook::xplat::module::CxxModule::Callback success, facebook::xplat::module::CxxModule::Callback error)
{
  m_nativeUIManager->configureNextLayoutAnimation(std::move(config), success, error);
}

void UIManager::createView(int64_t tag, std::string&& className, int64_t /*rootViewTag*/, folly::dynamic&& /*ReadableMap*/ props)
{
  m_nativeUIManager->ensureInBatch();
  auto viewManager = GetViewManager(className);
	auto node = viewManager->createShadow();
	node->m_className = std::move(className);
	node->m_tag = tag;
	node->m_viewManager = viewManager;

  node->createView();
  m_nativeUIManager->CreateView(*node, props);

  m_nodeRegistry.addNode(shadow_ptr(node), tag);

	if (!props.isNull())
		node->updateProperties(std::move(props));
}

void UIManager::setChildren(int64_t viewTag, folly::dynamic&& childrenTags)
{
  m_nativeUIManager->ensureInBatch();
  auto& parent = m_nodeRegistry.getNode(viewTag);
	int64_t index = 0;
	for (auto&& childTag : childrenTags)
	{
		auto tag = childTag.asInt();
		auto& childNode = m_nodeRegistry.getNode(tag);
		childNode.m_parent = parent.m_tag;
		parent.m_children.push_back(tag);
    if (!parent.m_zombie)
      parent.AddView(childNode, index);

    m_nativeUIManager->AddView(parent, childNode, index);

    ++index;
  }
}

void UIManager::updateView(int64_t tag, const std::string& className, folly::dynamic&& props)
{
  m_nativeUIManager->ensureInBatch();
	ShadowNode* pShadowNode = FindShadowNodeForTag(tag);
	// Guard against setNative calls to removed views
	if (pShadowNode == nullptr)
		return;

  if (!pShadowNode->m_zombie)
    pShadowNode->updateProperties(std::move(props));

  m_nativeUIManager->UpdateView(*pShadowNode, props);
}

void UIManager::RemoveShadowNode(ShadowNode& nodeToRemove)
{
	nodeToRemove.m_parent = -1;
	m_nodeRegistry.removeNode(nodeToRemove.m_tag);
	for (auto i = static_cast<int>(nodeToRemove.m_children.size()) - 1; i >= 0; --i)
	{
		RemoveShadowNode(m_nodeRegistry.getNode(nodeToRemove.m_children[i]));
	}

	nodeToRemove.m_children.clear();
}

void UIManager::replaceExistingNonRootView(int64_t oldTag, int64_t newTag)
{
  m_nativeUIManager->ensureInBatch();
  std::vector<int64_t> indicesToRemove(1);
	std::vector<int64_t> indicesToAdd(1);
	std::vector<int64_t> tagToAdd(1);
	tagToAdd[0] = newTag;

	CHECK(m_nodeRegistry.getNode(oldTag).m_parent != -1) << "oldTag must have a parent";
	auto& parent = m_nodeRegistry.getNode(m_nodeRegistry.getNode(oldTag).m_parent);
	auto it = find(parent.m_children.begin(), parent.m_children.end(), oldTag);
	CHECK(it != parent.m_children.end());
	indicesToAdd[0] = indicesToRemove[0] = it - parent.m_children.begin();

	std::vector<int64_t> emptyVec;
	manageChildren(parent.m_tag, emptyVec, emptyVec, tagToAdd, indicesToAdd, indicesToRemove);
}

void UIManager::dispatchViewManagerCommand(int64_t reactTag, int64_t commandId, folly::dynamic&& commandArgs)
{
  m_nativeUIManager->ensureInBatch();
  auto& node = m_nodeRegistry.getNode(reactTag);
  if (!node.m_zombie)
    node.dispatchCommand(commandId, std::move(commandArgs));
}

void UIManager::measure(int64_t reactTag, facebook::xplat::module::CxxModule::Callback callback)
{
  auto& node = m_nodeRegistry.getNode(reactTag);
  int64_t rootTag = reactTag;
  while (true)
  {
    auto& currNode = m_nodeRegistry.getNode(rootTag);
    if (currNode.m_parent == -1)
      break;
    rootTag = currNode.m_parent;
  }
  auto& rootNode = m_nodeRegistry.getNode(rootTag);

  m_nativeUIManager->measure(node, rootNode, callback);
}

void UIManager::onBatchComplete()
{
  m_nativeUIManager->onBatchComplete();
}

UIManagerModule::UIManagerModule(std::shared_ptr<IUIManager>&& manager)
	: m_manager(std::move(manager))
{
}

std::string UIManagerModule::getName()
{
	return "UIManager";
}

int64_t UIManager::AddMeasuredRootView(IReactRootView* rootView)
{
	auto tag = m_nextRootTag;
	m_nextRootTag += RootViewTagIncrement;

	int64_t width = rootView->GetActualWidth();
	int64_t height = rootView->GetActualHeight();

  RegisterRootView(rootView, tag, width, height);

	//TODO: call UpdateRootNodeSize when ReactRootView size changes
	/*var resizeCount = 0;
	rootView.SetOnSizeChangedListener((sender, args) =>
	{
	var currentCount = ++resizeCount;
	var newWidth = args.NewSize.Width;
	var newHeight = args.NewSize.Height;

	Context.RunOnNativeModulesQueueThread(() =>
	{
	if (currentCount == resizeCount)
	{
	Context.AssertOnNativeModulesQueueThread();
	_uiImplementation.UpdateRootNodeSize(tag, newWidth, newHeight, _eventDispatcher);
	}
	});
	});*/

	return tag;
}

std::unordered_set<int64_t>& UIManager::GetAllRootTags()
{
  return m_nodeRegistry.getAllRoots();
}

ShadowNode* UIManager::FindShadowNodeForTag(int64_t tag)
{
  return m_nodeRegistry.findNode(tag);
}

ShadowNode& UIManager::GetShadowNodeForTag(int64_t tag)
{
  return m_nodeRegistry.getNode(tag);
}

std::map<std::string, folly::dynamic> UIManagerModule::getConstants()
{
	std::map<std::string, folly::dynamic> constants {};

	m_manager->populateViewManagerConstants(constants);

	return constants;
}

std::vector<facebook::xplat::module::CxxModule::Method> UIManagerModule::getMethods()
{
	std::shared_ptr<IUIManager> manager(m_manager);
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
			manager->removeRootView(jsArgAsInt(args, 0));
		}),
		Method("createView", [manager](dynamic args)
		{
			manager->createView(jsArgAsInt(args, 0), jsArgAsString(args, 1), jsArgAsInt(args, 2), std::move(jsArgAsDynamic(args, 3)));
		}),
		Method("configureNextLayoutAnimation", [manager](dynamic args, Callback cbSuccess, Callback cbError)
		{
			manager->configureNextLayoutAnimation(std::move(jsArgAsDynamic(args, 0)), cbSuccess, cbError);
		}, AsyncTag),
		Method("setChildren", [manager](dynamic args)
		{
			manager->setChildren(jsArgAsInt(args, 0), std::move(jsArgAsArray(args, 1)));
		}),
		Method("updateView", [manager](dynamic args)
		{
			manager->updateView(jsArgAsInt(args, 0), jsArgAsString(args, 1), std::move(jsArgAsDynamic(args, 2)));
		}),
		Method("removeSubviewsFromContainerWithID", [manager](dynamic args)
		{
			manager->removeSubviewsFromContainerWithID(jsArgAsInt(args, 0));
		}),
		Method("manageChildren", [manager](dynamic args)
		{
			manager->manageChildren(jsArgAsInt(args, 0), jsArgAsDynamic(args, 1), jsArgAsDynamic(args, 2), jsArgAsDynamic(args, 3), jsArgAsDynamic(args, 4), jsArgAsDynamic(args, 5));
		}),
		Method("replaceExistingNonRootView", [manager](dynamic args)
		{
			manager->replaceExistingNonRootView(jsArgAsInt(args, 0), jsArgAsInt(args, 1));
		}),
		Method("dispatchViewManagerCommand", [manager](dynamic args)
		{
			manager->dispatchViewManagerCommand(jsArgAsInt(args, 0), jsArgAsInt(args, 1), std::move(jsArgAsDynamic(args, 2)));
		}),
    Method("measure", [manager](dynamic args, Callback cb)
    {
      manager->measure(jsArgAsInt(args, 0), cb);
    }),
    Method("setJSResponder", [manager](dynamic args)
    {
      // TODO: Implement?
      //manager->setJSResponder(jsArgAsInt(args, 0), jsArgAsBool(args, 1));
    }),
    Method("clearJSResponder", [this]()
    {
      // TODO: Implement?
      //manager->clearJSResponder();
    }),
	};
}

shared_ptr<IUIManager> createIUIManager(std::vector<std::unique_ptr<IViewManager>>&& viewManagers, INativeUIManager* nativeManager)
{
	return std::make_shared<UIManager>(std::move(viewManagers), nativeManager);
}

std::unique_ptr<facebook::xplat::module::CxxModule> createUIManagerModule(std::shared_ptr<IUIManager> uimanager)
{
	return std::make_unique<UIManagerModule>(std::move(uimanager));
}

}
} // namespace facebook::react
