// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <memory>
#include <vector>
#include <ViewManager.h>
#include <IUIManager.h>
#include <INativeUIManager.h>
#include <ShadowNodeRegistry.h>

namespace facebook { namespace react {

struct IReactRootView;
struct ShadowNode;

class UIManager : public IUIManager, INativeUIManagerHost
{
public:
	UIManager(std::vector<std::unique_ptr<IViewManager>>&& viewManagers, INativeUIManager* nativeManager);
	virtual ~UIManager() noexcept;

	void RegisterRootView(IReactRootView* rootView, int64_t rootViewTag, int64_t width, int64_t height);

	// IUIManager
	folly::dynamic getConstantsForViewManager(const std::string& className) override;
	void populateViewManagerConstants(std::map<std::string, folly::dynamic>& constants) override;
	void configureNextLayoutAnimation(folly::dynamic&& config, facebook::xplat::module::CxxModule::Callback success, facebook::xplat::module::CxxModule::Callback error) override;
	int64_t AddMeasuredRootView(IReactRootView* rootView) override;
	void removeRootView(int64_t rootViewTag) override;
	void createView(int64_t tag, std::string&& className, int64_t rootViewTag, folly::dynamic&& props) override;
	void setChildren(int64_t viewTag, folly::dynamic&& childrenTags) override;
	void updateView(int64_t tag, const std::string& className, folly::dynamic&& props) override;
	void removeSubviewsFromContainerWithID(int64_t containerTag) override;
	void manageChildren(int64_t viewTag, folly::dynamic& moveFrom, folly::dynamic& moveTo, folly::dynamic& addChildTags, folly::dynamic& addAtIndices, folly::dynamic& removeFrom) override;
	void dispatchViewManagerCommand(int64_t reactTag, int64_t commandId, folly::dynamic&& commandArgs) override;
	void replaceExistingNonRootView(int64_t oldTag, int64_t newTag) override;
	void onBatchComplete() override;
	void measure(int64_t reactTag, facebook::xplat::module::CxxModule::Callback callback) override;
	INativeUIManager* getNativeUIManager() override { return m_nativeUIManager; }

  void focus(int64_t reactTag) override;
  void blur(int64_t reactTag) override;
	// INativeUIManagerHost
	void zombieView(int64_t tag) override;
	std::unordered_set<int64_t>& GetAllRootTags() override;
	ShadowNode& GetShadowNodeForTag(int64_t tag) override;
	ShadowNode* FindShadowNodeForTag(int64_t tag) override;
  ShadowNode* FindParentRootShadowNode(int64_t tag) override;
private:
	std::vector<std::unique_ptr<IViewManager>> m_viewManagers;
	ShadowNodeRegistry m_nodeRegistry;
	INativeUIManager* m_nativeUIManager;

	void manageChildren(int64_t viewTag, std::vector<int64_t>& moveFrom, std::vector<int64_t>& moveTo, std::vector<int64_t>& addChildTags, std::vector<int64_t>& addAtIndices, std::vector<int64_t>& removeFrom);
	void RemoveShadowNode(ShadowNode& nodeToRemove);
	void DropView(int64_t tag, bool removeChildren = true, bool zombieView = false);
	IViewManager* GetViewManager(const std::string& className) const;

	int64_t m_nextRootTag = 101;
	static const int64_t RootViewTagIncrement = 10;
};

class UIManagerModule : public facebook::xplat::module::CxxModule
{
public:
	UIManagerModule(std::shared_ptr<IUIManager>&& manager);

	// CxxModule
	std::string getName() override;
	std::map<std::string, folly::dynamic> getConstants() override;
	std::vector<Method> getMethods() override;

private:
	std::shared_ptr<IUIManager> m_manager;
};

} }
