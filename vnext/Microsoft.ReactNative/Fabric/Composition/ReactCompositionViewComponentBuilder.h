#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <Fabric/AbiComponentDescriptor.h>
#include <Fabric/AbiViewComponentDescriptor.h>
#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include <react/renderer/core/ReactPrimitives.h>
#include "winrt/Microsoft.ReactNative.Composition.Experimental.h"
#include "winrt/Microsoft.ReactNative.Composition.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::Composition {

struct ReactCompositionViewComponentBuilder : winrt::implements<
                                                  ReactCompositionViewComponentBuilder,
                                                  IReactViewComponentBuilder,
                                                  Composition::IReactCompositionViewComponentBuilder> {
  ReactCompositionViewComponentBuilder() noexcept;

 public: // IReactViewComponentBuilder
  void SetComponentViewInitializer(const ComponentViewInitializer &initializer) noexcept;
  void SetCreateProps(ViewPropsFactory impl) noexcept;

  // (Object handle, Microsoft.ReactNative.IComponentState state) => void
  // void SetStateUpdater(StateUpdater impl) noexcept;

  void SetCreateShadowNode(ViewShadowNodeFactory impl) noexcept;
  void SetShadowNodeCloner(ViewShadowNodeCloner impl) noexcept;
  void SetInitialStateDataFactory(InitialStateDataFactory impl) noexcept;
  void SetMeasureContentHandler(MeasureContentHandler impl) noexcept;
  void SetLayoutHandler(LayoutHandler impl) noexcept;

  void SetCustomCommandHandler(HandleCommandDelegate impl) noexcept;
  void SetFinalizeUpdateHandler(UpdateFinalizerDelegate impl) noexcept;
  void SetUpdatePropsHandler(UpdatePropsDelegate impl) noexcept;
  void SetUpdateStateHandler(UpdateStateDelegate impl) noexcept;
  void SetUpdateEventEmitterHandler(UpdateEventEmitterDelegate impl) noexcept;
  void SetMountChildComponentViewHandler(MountChildComponentViewDelegate impl) noexcept;
  void SetUnmountChildComponentViewHandler(UnmountChildComponentViewDelegate impl) noexcept;

 public: // Composition::IReactCompositionViewComponentBuilder
  void SetViewComponentViewInitializer(const ViewComponentViewInitializer &initializer) noexcept;
  void SetContentIslandComponentViewInitializer(const ComponentIslandComponentViewInitializer &initializer) noexcept;

  void SetCreateVisualHandler(CreateVisualDelegate impl) noexcept;

 public:
  IComponentProps CreateProps(ViewProps props) noexcept;
  void CreateShadowNode(ShadowNode shadowNode) noexcept;
  void CloneShadowNode(ShadowNode shadowNode, ShadowNode sourceShadowNode) noexcept;
  winrt::Windows::Foundation::IInspectable InitialStateData(
      winrt::Microsoft::ReactNative::IComponentProps props) noexcept;
  MeasureContentHandler MeasureContentHandler() const noexcept;
  LayoutHandler LayoutHandler() const noexcept;
  facebook::react::ComponentDescriptorConstructor *GetComponentDescriptorProvider() const noexcept;

  winrt::Microsoft::ReactNative::ComponentView CreateView(
      const IReactContext &reactContext,
      facebook::react::Tag tag,
      const Experimental::ICompositionContext &context) noexcept;

 private:
  void InitializeComponentView(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept;

  ViewPropsFactory m_propsFactory;
  ViewShadowNodeFactory m_shadowNodeFactory;
  ViewShadowNodeCloner m_shadowNodeCloner;
  InitialStateDataFactory m_initialStateDataFactory;
  winrt::Microsoft::ReactNative::MeasureContentHandler m_measureContent;
  winrt::Microsoft::ReactNative::LayoutHandler m_layoutHandler;
  std::function<winrt::Microsoft::ReactNative::ComponentView(
      const IReactContext &reactContext,
      int32_t tag,
      const Experimental::ICompositionContext &context)>
      m_fnCreateView;
  std::function<facebook::react::ComponentDescriptorConstructor *()> m_descriptorConstructorFactory;
  winrt::Microsoft::ReactNative::HandleCommandDelegate m_customCommandHandler;
  winrt::Microsoft::ReactNative::UpdateFinalizerDelegate m_finalizeUpdateHandler;
  winrt::Microsoft::ReactNative::UpdatePropsDelegate m_updatePropsHandler;
  winrt::Microsoft::ReactNative::UpdateStateDelegate m_updateStateHandler;
  winrt::Microsoft::ReactNative::UpdateEventEmitterDelegate m_updateEventEmitterHandler;
  winrt::Microsoft::ReactNative::MountChildComponentViewDelegate m_mountChildComponentViewHandler;
  winrt::Microsoft::ReactNative::UnmountChildComponentViewDelegate m_unmountChildComponentViewHandler;

  winrt::Microsoft::ReactNative::Composition::CreateVisualDelegate m_createVisualHandler;
};

} // namespace winrt::Microsoft::ReactNative::Composition
