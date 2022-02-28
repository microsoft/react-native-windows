#pragma once
#include <DispatcherQueue.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>

class CompositionHost {
 public:
  CompositionHost();
  ~CompositionHost();
  static CompositionHost *GetInstance();

  void Initialize(HWND hwnd);
  void AddElement(float size, float x, float y);

  winrt::Windows::UI::Composition::Compositor Compositor() const noexcept;
  winrt::Windows::UI::Composition::Visual RootVisual() const noexcept;
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget Target() const noexcept;

 private:
  void CreateDesktopWindowTarget(HWND window);
  void EnsureDispatcherQueue();
  void CreateCompositionRoot();

  winrt::Windows::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{nullptr};
  winrt::Windows::System::DispatcherQueueController m_dispatcherQueueController{nullptr};
};
