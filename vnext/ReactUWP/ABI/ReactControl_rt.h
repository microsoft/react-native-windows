// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <wrl.h>

#include <ReactControl.h>
#include <windows.ui.xaml.h>

#include <wrl\client.h>
#include <wrl\wrappers\corewrappers.h>
#include <functional>

#include <IReactInstance.h>
#include <Views/ReactControl.h>
#include "IXamlRootView.h"

namespace react {
namespace uwp {
struct IInstanceManager;
}
} // namespace react

namespace ABI {
namespace react {
namespace uwp {

class ReactControl : public Microsoft::WRL::RuntimeClass<
                         ABI::react::uwp::IReactControl,
                         Microsoft::WRL::ComposableBase<
                             ABI::Windows::UI::Xaml::Controls::IGridFactory>>,
                     public ::react::uwp::IXamlRootView {
  InspectableClass(RuntimeClass_react_uwp_ReactControl, BaseTrust);

 public:
  ReactControl();

  virtual HRESULT STDMETHODCALLTYPE get_Instance(IInstance **value) override;
  virtual HRESULT STDMETHODCALLTYPE put_Instance(IInstance *value) override;
  virtual HRESULT STDMETHODCALLTYPE
  get_JsComponentName(HSTRING *value) override;
  virtual HRESULT STDMETHODCALLTYPE put_JsComponentName(HSTRING value) override;
  virtual HRESULT STDMETHODCALLTYPE get_InitialProps(HSTRING *value) override;
  virtual HRESULT STDMETHODCALLTYPE put_InitialProps(HSTRING value) override;
  virtual HRESULT STDMETHODCALLTYPE StartRender() override;
  virtual HRESULT STDMETHODCALLTYPE StopRender() override;

  void Refresh();
  void CreateInnerInstance();

  // IReactRootView implementations
  virtual std::shared_ptr<::react::uwp::IReactInstance> GetReactInstance() const
      noexcept override;
  virtual ::react::uwp::XamlView GetXamlView() const noexcept override;

  virtual void SetJSComponentName(
      std::string &&mainComponentName) noexcept override;
  virtual void SetInstanceCreator(const ::react::uwp::ReactInstanceCreator
                                      &instanceCreator) noexcept override;
  virtual void SetInitialProps(folly::dynamic &&initialProps) noexcept override;

  virtual void AttachRoot() noexcept override;
  virtual void DetachRoot() noexcept override;

  virtual std::shared_ptr<::react::uwp::IXamlReactControl> GetXamlReactControl()
      const noexcept override;

  // IReactRootView implementations
  virtual void ResetView() override;
  virtual std::string JSComponentName() const noexcept override;
  virtual int64_t GetActualHeight() const override;
  virtual int64_t GetActualWidth() const override;
  virtual int64_t GetTag() const override;
  virtual void SetTag(int64_t tag) override;

 private:
  Microsoft::WRL::ComPtr<IInspectable> _spInnerInspectable;

  std::string m_jsComponentName;
  std::string m_initialProps;

  Microsoft::WRL::ComPtr<IInstance> _spInstance;
  bool m_rendering;

  std::shared_ptr<::react::uwp::ReactControl> m_pInnerControl;
};

class ReactControlFactory
    : public Microsoft::WRL::AgileActivationFactory<IReactControlFactory> {
 public:
  virtual HRESULT STDMETHODCALLTYPE
  ActivateInstance(_Outptr_ IInspectable **ppInstance) override;
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(
      _In_opt_ IInspectable *pOuter,
      _Outptr_opt_ IInspectable **ppInner,
      _Outptr_ ABI::react::uwp::IReactControl **ppInstance);
};

} // namespace uwp
} // namespace react
} // namespace ABI
