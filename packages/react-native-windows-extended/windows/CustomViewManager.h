/* TODO: Update or remove this example to reflect the new ViewManager architecture provided by Microsoft.ReactNative. */

#pragma once

// Examples of defining a custom ViewManager
// NOTE: ViewManagers will be deprecated in the future with fabric
// rearchitecture that will
//   move things to be more shadownode based.

#include <ReactUWP/ReactUwp.h>
#include <ReactUWP/Views/FrameworkElementViewManager.h>

class CustomFrameworkElementViewManager : public react::uwp::FrameworkElementViewManager {
  using Super = react::uwp::FrameworkElementViewManager;

 public:
  CustomFrameworkElementViewManager(const std::shared_ptr<react::uwp::IReactInstance> &reactInstance);

  const char *GetName() const override;
  folly::dynamic GetCommands() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  facebook::react::ShadowNode *createShadow() const override;

  void UpdateProperties(react::uwp::ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) override;
  void DispatchCommand(react::uwp::XamlView viewToUpdate, int64_t commandId, const folly::dynamic &commandArgs)
      override;

 protected:
  react::uwp::XamlView CreateViewCore(int64_t tag) override;

  friend class CustomFrameworkElementShadowNode;
};
