// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "codegen/react/components/SampleCustomComponent/CustomTextInput.g.h"

#ifdef RNW_NEW_ARCH

#include "ComponentView.Experimental.interop.h"

#include <Richedit.h>

#include <textserv.h>
#include <tom.h>

namespace winrt::SampleCustomComponent {

struct CustomTextInputComponentView : public winrt::implements<CustomTextInputComponentView, winrt::IInspectable>,
                                      Codegen::BaseCustomTextInput<CustomTextInputComponentView> {
  void InitializeTextInput(const winrt::Microsoft::ReactNative::Composition::WindowsTextInputComponentView &textInput) {
    auto textProps = textInput.ViewProps().as<winrt::Microsoft::ReactNative::TextInputProps>();
    auto secureTextEntry = textProps.SecureTextEntry(); // Test we can get a TextInput specific property

    auto textInputInterop =
        textInput.as<::Microsoft::ReactNative::Composition::Experimental::ITextInputComponentInterop>();

    // Verify we can get the ITextDocument2 and ITextServices2 for the TextInput
    winrt::com_ptr<ITextDocument2> textDoc;
    textInputInterop->GetTextDocument(textDoc.put());
    winrt::com_ptr<ITextServices2> textServices;
    textInputInterop->GetTextServices(textServices.put());
  }

  void HandleSetLightOnCommand(bool) noexcept override {
    // Handle the command here
  }
};

} // namespace winrt::SampleCustomComponent

void RegisterCustomTextInputComponentView(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  winrt::SampleCustomComponent::Codegen::RegisterCustomTextInputNativeComponent<
      winrt::SampleCustomComponent::CustomTextInputComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetTextInputComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::WindowsTextInputComponentView
                   &textInputView) noexcept {
              auto userData = winrt::make_self<winrt::SampleCustomComponent::CustomTextInputComponentView>();
              userData->InitializeTextInput(textInputView);
              textInputView.UserData(*userData);
            });
      });
}

#endif // RNW_NEW_ARCH
