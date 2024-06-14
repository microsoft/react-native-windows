#pragma once

#include <Fabric/Composition/RootComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>
#include <inspectable.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactNativeIsland;

class CompositionRootAutomationProvider : public winrt::implements<
                                              CompositionRootAutomationProvider,
                                              IInspectable,
                                              IRawElementProviderFragmentRoot,
                                              IRawElementProviderFragment,
                                              IRawElementProviderSimple,
                                              IRawElementProviderAdviseEvents> {
 public:
  // inherited via IRawElementProviderFragmentRoot
  virtual HRESULT __stdcall ElementProviderFromPoint(double x, double y, IRawElementProviderFragment **pRetVal)
      override;
  virtual HRESULT __stdcall GetFocus(IRawElementProviderFragment **pRetVal) override;

  // inherited via IRawElementProviderFragment
  virtual HRESULT __stdcall Navigate(NavigateDirection direction, IRawElementProviderFragment **pRetVal) override;
  virtual HRESULT __stdcall GetRuntimeId(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall get_BoundingRectangle(UiaRect *pRetVal) override;
  virtual HRESULT __stdcall GetEmbeddedFragmentRoots(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall SetFocus(void) override;
  virtual HRESULT __stdcall get_FragmentRoot(IRawElementProviderFragmentRoot **pRetVal) override;

  // inherited via IRawElementProviderSimple
  virtual HRESULT __stdcall get_ProviderOptions(ProviderOptions *pRetVal) override;
  virtual HRESULT __stdcall GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal) override;
  virtual HRESULT __stdcall GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal) override;
  virtual HRESULT __stdcall get_HostRawElementProvider(IRawElementProviderSimple **pRetVal) override;

  // IRawElementProviderAdviseEvents
  virtual HRESULT __stdcall AdviseEventAdded(EVENTID idEvent, SAFEARRAY *psaProperties) override;
  virtual HRESULT __stdcall AdviseEventRemoved(EVENTID idEvent, SAFEARRAY *psaProperties) override;

  CompositionRootAutomationProvider(const winrt::Microsoft::ReactNative::ReactNativeIsland &rootView) noexcept;

  void SetHwnd(HWND hwnd) noexcept;
#ifdef USE_WINUI3
  void SetIsland(winrt::Microsoft::UI::Content::ContentIsland &island) noexcept;
#endif
  bool WasPropertyAdvised(PROPERTYID prop) noexcept;
  bool WasEventAdvised(EVENTID event) noexcept;

  // It's unlikely for the underlying primitive types for EVENTID and PROPERTYID to ever change, but let's make sure
  static_assert(std::is_same<EVENTID, PROPERTYID>::value);
  // Helper class for AdviseEventAdded/Removed. I could've simply used a std::pair, but I find using structs with named
  // members easier to read and more self-documenting than pair.first and pair.last. Since this is simply syntactic
  // sugar, I'm leveraging the fact that both EVENTID and PROPERTYID are ints under the hood to share
  // AdviseEventAddedImpl
  struct AdvisedEvent {
    union {
      EVENTID Event;
      PROPERTYID Property;
    };
    uint32_t Count;
  };

 private:
  winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *rootComponentView() noexcept;

  HRESULT AdvisePropertiesAdded(SAFEARRAY *psaProperties) noexcept;
  HRESULT AdvisePropertiesRemoved(SAFEARRAY *psaProperties) noexcept;

  // Linear search on unsorted vectors is typically faster than more sophisticated data structures when N is small. In
  // practice ATs tend to only listen to a dozen or so props and events, so std::vector is likely better than maps.
  std::vector<AdvisedEvent> m_advisedEvents{};
  std::vector<AdvisedEvent> m_advisedProperties{};
  winrt::weak_ref<winrt::Microsoft::ReactNative::ReactNativeIsland> m_wkRootView;
  HWND m_hwnd{nullptr};
#ifdef USE_WINUI3
  winrt::Microsoft::UI::Content::ContentIsland m_island{nullptr};
#endif
};

} // namespace winrt::Microsoft::ReactNative::implementation
