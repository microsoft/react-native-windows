// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "codegen/react/components/SampleCustomComponent/CustomTextInput.g.h"

#ifdef RNW_NEW_ARCH

#include "ComponentView.Experimental.interop.h"

#include <Richedit.h>

#include <textserv.h>
#include <tom.h>
#include <winrt/Windows.Data.Json.h>
#include <memory>

namespace winrt::SampleCustomComponent {

enum class ODeltaListEndType : int64_t { Parenthesis, Period, Plain, Minus };

enum class ODeltaSubscriptSuperscript : int64_t { None, Subscript, Superscript };

// Function to convert winrt::hstring to BSTR
std::unique_ptr<wchar_t, decltype(&SysFreeString)> HStringToBSTR(const winrt::hstring &hstr) {
  // Get the raw wide string pointer and length
  const wchar_t *raw = hstr.c_str();
  UINT length = static_cast<UINT>(hstr.size());

  // Allocate a BSTR with the same content
  BSTR bstr = SysAllocStringLen(raw, length);
  if (!bstr) {
    throw std::bad_alloc(); // Handle allocation failure
  }
  return {bstr, &SysFreeString};
}

void ReplaceAllOfWchWithWch(WCHAR *wz, WCHAR wchOut, WCHAR wchIn) noexcept {
  while (*wz != 0) {
    if (*wz == wchOut)
      *wz = wchIn;

    ++wz;
  }
}

// RAII helper that groups a sequence of RichEdit edits into a single undo/redo unit.
// Ownership is tracked via m_spTextDoc, which is only set when this object successfully begins
// the edit collection. A nested object whose BeginEditCollection returns S_FALSE (a collection is
// already in progress) leaves m_spTextDoc null, so only the top-level owning object ends it.
class AutoEditCollection final {
 public:
  AutoEditCollection(const winrt::com_ptr<ITextDocument2> &spTextDoc) noexcept;
  ~AutoEditCollection() noexcept;

  AutoEditCollection(const AutoEditCollection &) = delete;
  const AutoEditCollection &operator=(const AutoEditCollection &) = delete;

  // Default constructor with a separate init makes it easier to add AutoEditCollection with a CG check.
  AutoEditCollection() noexcept = default;
  void InitWithDocument(const winrt::com_ptr<ITextDocument2> &spTextDoc) noexcept;

 private:
  void BeginEditCollection(const winrt::com_ptr<ITextDocument2> &spTextDoc) noexcept;
  winrt::com_ptr<ITextDocument2> m_spTextDoc;
};

AutoEditCollection::AutoEditCollection(const winrt::com_ptr<ITextDocument2> &spTextDoc) noexcept {
  BeginEditCollection(spTextDoc);
}

void AutoEditCollection::InitWithDocument(const winrt::com_ptr<ITextDocument2> &spTextDoc) noexcept {
  if (m_spTextDoc) {
    assert(false);
    return;
  }

  BeginEditCollection(spTextDoc);
}

AutoEditCollection::~AutoEditCollection() noexcept {
  if (m_spTextDoc) {
    m_spTextDoc->EndEditCollection();
  }
}

void AutoEditCollection::BeginEditCollection(const winrt::com_ptr<ITextDocument2> &spTextDoc) noexcept {
  if (!spTextDoc) {
    assert(false);
    return;
  }

  // Only take ownership of (and later end) the edit collection when we successfully begin it.
  // BeginEditCollection returns S_FALSE when a collection is already in progress; S_FALSE is not
  // caught by the FAILED macro, so only S_OK counts as a successful, owning begin.
  if (spTextDoc->BeginEditCollection() == S_OK) {
    m_spTextDoc = spTextDoc;
  }
}

struct CustomTextInputComponentView : public winrt::implements<CustomTextInputComponentView, winrt::IInspectable>,
                                      Codegen::BaseCustomTextInput<CustomTextInputComponentView> {
  void InitializeTextInput(const winrt::Microsoft::ReactNative::Composition::WindowsTextInputComponentView &textInput) {
    m_wkTextInputView = textInput;
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

  void HandleSetFormattedTextCommand(std::string json) noexcept override {
    auto textBox = m_wkTextInputView.get();
    if (textBox == nullptr) {
      assert(false);
      return;
    }

    auto textInputInterop =
        textBox.as<::Microsoft::ReactNative::Composition::Experimental::ITextInputComponentInterop>();
    winrt::com_ptr<ITextDocument2> spTextDoc2;
    textInputInterop->GetTextDocument(spTextDoc2.put());

    if (!spTextDoc2) {
      assert(false);
      return;
    }

#if 0
    // TOM interfaces don't allow programmatic edits if readonly. The following is a workaround.
    bool revertEditable { !textBox->IsEditable() };
    if (revertEditable)
    {
      textBox->EnableEditable();
    }

    auto textboxView = textBox->GetView();
    auto langOptions = textboxView->GetLangOptions();
    bool revertLangOptions { !!(langOptions & IMF_SPELLCHECKING) };
    if (revertLangOptions)
    {
      textboxView->SetLangOptions(langOptions & ~IMF_SPELLCHECKING);
    }
    auto controller = textBox->GetController();
    if (controller)
      controller->SetProcessingProgrammaticSelChange(true);			// avoid selection change triggers when setting delta

    bool isClickableUrl { textBox->GetEnableClickableUrl() };
    bool isClickableEmail { textBox->GetEnableClickableEmailAddr() };
    // need to set scanExisting flag to true (which removes the AURL_NOINITIALSCAN flag) to make sure RE resets the autourl detector
    textboxView->SetAutoUrlDetect(false /*fEnableClickableUrl*/, false /*fEnableClickableEmailAddr*/, true /*scanExisting*/);

#endif
    winrt::check_hresult(spTextDoc2->SetNotificationMode(tomFalse)); // disable all notifications for perf
    bool textSet = false;

    {
      AutoEditCollection editCollection(spTextDoc2); // combine the entire delta into a single undo/redo event
      winrt::check_hresult(spTextDoc2->Freeze(nullptr)); // freeze/unfreeze to avoid display flicker

      /*
      if (reset)
      {
        ResetTextbox();
      }
        */

      textSet = SetTextFromDeltaCore(json);

      if (spTextDoc2->Unfreeze(nullptr) != S_OK) {
        assert(false);
      }
    }

    winrt::check_hresult(spTextDoc2->SetNotificationMode(tomTrue));
    winrt::check_bool(textSet);

    // textboxView->SetAutoUrlDetect(isClickableUrl, isClickableEmail, false /*scanExisting*/);

    // Normally TxNotify/EN_CHANGE is called when WLTB gets change notifications from RichEdit.
    // But we had disabled notifications, so we need to explicitly call TxNotify to process text change after
    // SetTextFromDeltaCore.
    winrt::com_ptr<ITextHost> spTextHost;
    winrt::check_hresult(textInputInterop->GetTextHost(spTextHost.put()));
    winrt::check_bool(static_cast<bool>(spTextHost));

    CHANGENOTIFY changeNotify{CN_TEXTCHANGED, nullptr};
    spTextHost->TxNotify(EN_CHANGE, &changeNotify);

    /*
    if (controller)
    {
      controller->SetProcessingProgrammaticSelChange(false);
    }
    */

    // Same reasoning as the EN_CHANGE message above
    if (winrt::com_ptr<ITextSelection2> spTextSel; SUCCEEDED(spTextDoc2->GetSelection2(spTextSel.put())) && spTextSel) {
      if (LONG start{0}, end{0}; SUCCEEDED(spTextSel->GetStart(&start)) && SUCCEEDED(spTextSel->GetEnd(&end))) {
        // setting text via replaceText at an empty selection/IP position can cause selection to be non-empty which we
        // don't want. so always indicate selection to be empty and at the end character position of the inserted text.
        // this also fixes an issue in Copilot where we expect selection to be an IP at the end of the inserted text.
        SELCHANGE selChangeNotify{{}, {end, end}, SEL_EMPTY};
        spTextHost->TxNotify(EN_SELCHANGE, &selChangeNotify);
      }
    }

    /*
    if (reset)
    {
      // resetting textbox should reset the undo stack as well
      if (FAILED(spTextDoc2->Undo(0, nullptr)) || FAILED(spTextDoc2->Undo(-1, nullptr)))
        assert(false);
    }

    if (revertLangOptions)
    {
      textboxView->SetLangOptions(langOptions);
    }

    if (revertEditable)
    {
      textBox->DisableEditable();
    }
    */
  }

  bool SetTextFromDeltaCore(const std::string &json) noexcept {
    auto textBox = m_wkTextInputView.get();
    if (textBox == nullptr) {
      assert(false);
      return false;
    }

    auto textInputInterop =
        textBox.as<::Microsoft::ReactNative::Composition::Experimental::ITextInputComponentInterop>();
    winrt::com_ptr<ITextDocument2> spTextDoc2;
    textInputInterop->GetTextDocument(spTextDoc2.put());

    if (!spTextDoc2) {
      assert(false);
      return false;
    }

    winrt::com_ptr<ITextRange2> spRange2;
    if (FAILED(spTextDoc2->Range2(0, tomForward, spRange2.put())) || spRange2 == nullptr)
      return false;
    // exclude the final (special) EOP which causes issues when setting first para to RTL
    if (FAILED(spRange2->MoveEnd(tomCharacter, -1, nullptr)))
      return false;

    winrt::Windows::Data::Json::JsonObject delta;
    if (!winrt::Windows::Data::Json::JsonObject::TryParse(winrt::to_hstring(json), delta)) {
      assert(false);
      return false;
    }

    if (!SetTextRangeFromDelta(*spRange2, delta)) {
      assert(false);
      return false;
    }
    return true;
  }

  bool SetTextRangeFromDelta(ITextRange2 &range2, const winrt::Windows::Data::Json::JsonObject &delta) noexcept {
    auto opsPtr = delta.GetNamedArray(L"ops");
    if (!opsPtr) {
      assert(false);
      return false;
    }

    bool isFirstOp{true};
    for (const auto &operationValue : opsPtr) {
      auto deltaOperation = operationValue.GetObject();
      auto insertValue = deltaOperation.GetNamedValue(L"insert", nullptr);
      if (!insertValue) {
        assert(false);
        return false;
      }
      if (insertValue.ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
        if (!SetTextRangeFromTextOpsDelta(range2, deltaOperation, isFirstOp)) {
          assert(false);
          return false;
        }
        isFirstOp = false;
      } else {
        // NYI test impl
        assert(false);
        return false;
      }
    }

    if (ResetFont(range2) == nullptr)
      return false;
    return true;
  }

  winrt::com_ptr<ITextFont2> ResetFont(ITextRange2 &range2) noexcept {
    winrt::com_ptr<ITextFont2> spTextFont;
    if (FAILED(range2.GetFont2(spTextFont.put())) || spTextFont == nullptr ||
        FAILED(spTextFont->SetProtected(tomFalse)) || FAILED(spTextFont->Reset(tomDefault)))
      return nullptr;
    return spTextFont;
  }

  bool SetFontFromDeltaAttribute(ITextFont2 &font, const winrt::Windows::Data::Json::JsonObject &attributes) noexcept {
    if (attributes.HasKey(L"weight") && FAILED(font.SetWeight(static_cast<int>(attributes.GetNamedNumber(L"weight")))))
      return false;
    if (attributes.HasKey(L"italic") &&
        FAILED(font.SetItalic(attributes.GetNamedBoolean(L"italic") ? tomTrue : tomFalse)))
      return false;
    if (attributes.HasKey(L"underline") &&
        FAILED(font.SetUnderline(attributes.GetNamedBoolean(L"underline") ? tomSingle : tomNone)))
      return false;
    if (attributes.HasKey(L"strikethrough") &&
        FAILED(font.SetStrikeThrough(attributes.GetNamedBoolean(L"strikethrough") ? tomTrue : tomFalse)))
      return false;
    if (attributes.HasKey(L"size") && FAILED(font.SetSize(static_cast<float>(attributes.GetNamedNumber(L"size")))))
      return false;
    if (attributes.HasKey(L"fontFamily")) {
      auto name = HStringToBSTR(attributes.GetNamedString(L"fontFamily"));
      if (FAILED(font.SetName(name.get())))
        return false;
    }
    // TODO support color, highlight, subscriptAndSuperscript,
    return true;
  }

  bool SetTextRangeFromTextOpsDelta(
      ITextRange2 &range2,
      const winrt::Windows::Data::Json::JsonObject &delta,
      bool isFirstOp) noexcept {
    auto spTextFont = ResetFont(range2);
    if (!spTextFont)
      return false;

    auto attributesValue = delta.GetNamedValue(L"attributes", nullptr);
    auto attributesPtr = attributesValue ? attributesValue.GetObject() : nullptr;
    if (attributesPtr) {
      if (!SetParagraphFromDeltaAttribute(range2, attributesPtr, isFirstOp)) {
        assert(false);
        return false;
      }
    }

    // set text
    auto insertValue = delta.Lookup(L"insert");
    auto insertStr = insertValue.GetString();

    auto bstr = HStringToBSTR(insertStr);
    if (bstr && SysStringLen(bstr.get()) != 0) {
      // new line in RE is \v.
      ReplaceAllOfWchWithWch(bstr.get(), L'\n', L'\v');
      ReplaceAllOfWchWithWch(bstr.get(), L'\r', L'\v');
    }
    if (FAILED(range2.SetText(bstr.get()))) {
      return false;
    }

    if (attributesPtr) {
      // dup font - this is more efficient than setting individual props
      winrt::com_ptr<ITextFont2> spTextFontDup;
      if (FAILED(spTextFont->GetDuplicate2(spTextFontDup.put())) || spTextFontDup == nullptr)
        return false;

      if (!SetFontFromDeltaAttribute(*spTextFontDup, attributesPtr)) {
        assert(false);
        return false;
      }

      if (FAILED(range2.SetFont(spTextFontDup.get())))
        return false;

      if (auto language = attributesPtr.GetNamedValue(L"language", nullptr)) {
        auto bstrLanguage = HStringToBSTR(language.GetString());
        if (FAILED(range2.SetText2(tomLanguageTag, bstrLanguage.get()))) {
          return false;
        }
      }

      // set url - this changes range cp, so do this last
      if (!SetLinkFromDeltaAttribute(range2, attributesPtr)) {
        assert(false);
        return false;
      }
    }

    if (FAILED(range2.Move(tomCharacter, 1, nullptr)))
      return false;
    return true;
  }

  bool SetParagraphFromDeltaAttribute(
      ITextRange2 &range2,
      const winrt::Windows::Data::Json::JsonObject &deltaAttr,
      bool isFirstPara) noexcept {
    auto paraValue = deltaAttr.GetNamedValue(L"paragraph", nullptr);
    if (!paraValue)
      return true;
    auto paraPtr = paraValue.GetObject();

    if (!isFirstPara) {
      // start paragraph
      auto bstr = HStringToBSTR(L"\r");
      if (FAILED(range2.SetText(bstr.get())) || FAILED(range2.Move(tomCharacter, 1, nullptr))) {
        return false;
      }
    }

    // default para options
    LONG listType{tomListNone};
    LONG alignTom{tomAlignLeft};
    LONG listLevel{0};
    float indent{0};
    bool isRTLDirection{false};

    // set paragraph/list options
    winrt::com_ptr<ITextPara2> spTextPara2;
    if (FAILED(range2.GetPara2(spTextPara2.put())) || !spTextPara2 || FAILED(spTextPara2->Reset(tomDefault)))
      return false;

    if (auto listTypePtr = paraPtr.GetNamedValue(L"listType", nullptr)) {
      listType = static_cast<LONG>(listTypePtr.GetNumber());
      assert(listType >= tomListNone && listType <= tomListNumberAsUCRoman);
    }

    if (auto listEndTypePtr = paraPtr.GetNamedValue(L"listEndType", nullptr)) {
      assert(listType != tomListNone);
      auto listEndType = GetTomListEndType(static_cast<int64_t>(listEndTypePtr.GetNumber()));
      listType |= listEndType;
    }

    if (auto listLevelPtr = paraPtr.GetNamedValue(L"listLevel", nullptr)) {
      assert(listType != tomListNone);
      listLevel = static_cast<LONG>(listLevelPtr.GetNumber());
      assert(listLevel >= 0 && (listLevel == 0 || listType != tomListNone));
    }

    if (auto alignPtr = paraPtr.GetNamedValue(L"alignment", nullptr)) {
      alignTom = static_cast<LONG>(alignPtr.GetNumber());
      assert(alignTom >= tomAlignLeft && alignTom <= tomAlignJustify);
    }

    if (auto indentPtr = paraPtr.GetNamedValue(L"indent", nullptr)) {
      indent = static_cast<float>(indentPtr.GetNumber());
      if (indent < 0.0f) {
        // odelta expects positive indents
        indent = 0.0f;
      }
    }

    if (auto directionPtr = paraPtr.GetNamedValue(L"directionRTL", nullptr)) {
      isRTLDirection = directionPtr.GetBoolean();
    }

    // SetEffects(tomParaEffectRTL, ..) implicitly sets the alignment to right. Do SetAlignment after SetEffects so that
    // left aligned RTL text is possible.
    if (FAILED(spTextPara2->SetListType(listType)) ||
        (listLevel && FAILED(spTextPara2->SetListLevelIndex(listLevel))) ||
        (isRTLDirection && FAILED(spTextPara2->SetEffects(tomParaEffectRTL, tomParaEffectRTL))) ||
        FAILED(spTextPara2->SetAlignment(alignTom)) ||
        FAILED(spTextPara2->SetIndents(
            tomUndefined, alignTom == tomAlignLeft ? indent : 0, alignTom == tomAlignRight ? indent : 0)))
      return false;

    // start numbered list from 1 instead of 0
    if (LONG curListStart{-1}; ((listType & 0xFFFF) == tomListNumberAsArabic) &&
        SUCCEEDED(spTextPara2->GetListStart(&curListStart)) && curListStart == 0) {
      if (FAILED(spTextPara2->SetListStart(1)))
        return false;
    }
    return true;
  }

  bool SetLinkFromDeltaAttribute(
      ITextRange2 &range2,
      const winrt::Windows::Data::Json::JsonObject &deltaAttr) noexcept {
    auto linkItr = deltaAttr.GetNamedValue(L"link", nullptr);
    if (!linkItr)
      return true;

    auto linkStr = std::wstring(linkItr.GetString());
    if (linkStr.empty() || linkStr.front() != L'"') {
      linkStr = L"\"" + linkStr + L"\"";
    }
    auto bstrURL = HStringToBSTR(winrt::hstring{linkStr});
    if (FAILED(range2.SetURL(bstrURL.get()))) {
      return false;
    }
    return true;
  }

  LONG GetTomListEndType(int64_t tomListEndType) noexcept {
    switch (static_cast<ODeltaListEndType>(tomListEndType)) {
      case ODeltaListEndType::Period:
        return tomListPeriod;
      case ODeltaListEndType::Plain:
        return tomListPlain;
      case ODeltaListEndType::Minus:
        return tomListMinus;
      default:
        assert(false);
        __fallthrough;
      case ODeltaListEndType::Parenthesis:
        return tomListParentheses;
    }
  }

 private:
  winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::WindowsTextInputComponentView> m_wkTextInputView;
};

} // namespace winrt::SampleCustomComponent

void RegisterCustomTextInputComponentView(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  winrt::SampleCustomComponent::Codegen::RegisterCustomTextInputNativeComponent<
      winrt::SampleCustomComponent::CustomTextInputComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        // Exercise userdata access during creation and cloning without replacing the native text state.
        auto shadowNodeBuilder = builder.as<winrt::Microsoft::ReactNative::IReactViewComponentBuilder>();
        shadowNodeBuilder.SetInitialStateDataFactory(
            [](const winrt::Microsoft::ReactNative::IComponentProps &) { return winrt::box_value(int32_t{0}); });
        shadowNodeBuilder.SetCreateShadowNode([](const winrt::Microsoft::ReactNative::ShadowNode &shadowNode) {
          winrt::check_bool(winrt::unbox_value<int32_t>(shadowNode.StateData()) == 0);
          shadowNode.StateData(winrt::box_value(int32_t{1}));
          winrt::check_bool(winrt::unbox_value<int32_t>(shadowNode.StateData()) == 1);
        });
        shadowNodeBuilder.SetShadowNodeCloner([](const winrt::Microsoft::ReactNative::ShadowNode &shadowNode,
                                                 const winrt::Microsoft::ReactNative::ShadowNode &sourceShadowNode) {
          auto sourceData = winrt::unbox_value<int32_t>(sourceShadowNode.StateData());
          winrt::check_bool(sourceData > 0);
          shadowNode.StateData(winrt::box_value(sourceData + 1));
          winrt::check_bool(winrt::unbox_value<int32_t>(shadowNode.StateData()) == sourceData + 1);
          winrt::check_bool(winrt::unbox_value<int32_t>(sourceShadowNode.StateData()) == sourceData);
        });
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
