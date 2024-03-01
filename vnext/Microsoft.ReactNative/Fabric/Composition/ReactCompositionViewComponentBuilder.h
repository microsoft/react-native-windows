#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <react/renderer/core/ReactPrimitives.h>
#include "winrt/Microsoft.ReactNative.Composition.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::Composition {

struct ReactCompositionViewComponentBuilder : winrt::implements<
                                                  ReactCompositionViewComponentBuilder,
                                                  IReactViewComponentBuilder,
                                                  Composition::IReactCompositionViewComponentBuilder> {
  ReactCompositionViewComponentBuilder() noexcept;

 public: // IReactViewComponentBuilder
  void SetCreateComponentView(ComponentViewFactory impl) noexcept;
  void SetCreateProps(ViewPropsFactory impl) noexcept;

  // (Object handle, Microsoft.ReactNative.IComponentState state) => void
  // void SetStateUpdater(StateUpdater impl) noexcept;

  void SetCreateShadowNode(ViewShadowNodeFactory impl) noexcept;
  void SetShadowNodeCloner(ViewShadowNodeCloner impl) noexcept;
  void SetInitialStateDataFactory(InitialStateDataFactory impl) noexcept;
  void SetMeasureContentHandler(MeasureContentHandler impl) noexcept;
  void SetLayoutHandler(LayoutHandler impl) noexcept;

 public: // Composition::IReactCompositionViewComponentBuilder
  void SetCreateViewComponentView(CompositionViewComponentViewFactory impl) noexcept;

 public:
  IComponentProps CreateProps(ViewProps props) noexcept;
  void CreateShadowNode(ShadowNode shadowNode) noexcept;
  void CloneShadowNode(ShadowNode shadowNode, ShadowNode sourceShadowNode) noexcept;
  winrt::Windows::Foundation::IInspectable InitialStateData(
      winrt::Microsoft::ReactNative::IComponentProps props) noexcept;
  MeasureContentHandler MeasureContentHandler() const noexcept;
  LayoutHandler LayoutHandler() const noexcept;
  bool IsViewComponent() const noexcept;

  winrt::Microsoft::ReactNative::ComponentView
  CreateView(const IReactContext &reactContext, facebook::react::Tag tag, const ICompositionContext &context) noexcept;

 private:
  ViewPropsFactory m_propsFactory;
  ViewShadowNodeFactory m_shadowNodeFactory;
  ViewShadowNodeCloner m_shadowNodeCloner;
  InitialStateDataFactory m_initialStateDataFactory;
  winrt::Microsoft::ReactNative::MeasureContentHandler m_measureContent;
  winrt::Microsoft::ReactNative::LayoutHandler m_layoutHandler;

  ComponentViewFactory m_createComponentView{nullptr};
  CompositionViewComponentViewFactory m_createView{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition
