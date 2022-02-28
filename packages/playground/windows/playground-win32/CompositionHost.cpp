#include "pch.h"
#include "CompositionHost.h"

using namespace winrt;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Composition::Desktop;
using namespace Windows::Foundation::Numerics;

CompositionHost::CompositionHost() {}

CompositionHost *CompositionHost::GetInstance() {
  static CompositionHost instance;
  return &instance;
}

CompositionHost::~CompositionHost() {}

void CompositionHost::Initialize(HWND hwnd) {
  EnsureDispatcherQueue();
  if (m_dispatcherQueueController)
    m_compositor = winrt::Windows::UI::Composition::Compositor();

  if (m_compositor) {
    CreateDesktopWindowTarget(hwnd);
    CreateCompositionRoot();
  }
}

winrt::Windows::UI::Composition::Compositor CompositionHost::Compositor() const noexcept {
  return m_compositor;
}

winrt::Windows::UI::Composition::Visual CompositionHost::RootVisual() const noexcept {
  return m_target.Root();
}

winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget CompositionHost::Target() const noexcept {
  return m_target;
}

void CompositionHost::EnsureDispatcherQueue() {
  namespace abi = ABI::Windows::System;

  if (m_dispatcherQueueController == nullptr) {
    DispatcherQueueOptions options{
        sizeof(DispatcherQueueOptions), /* dwSize */
        DQTYPE_THREAD_CURRENT, /* threadType */
        DQTAT_COM_ASTA /* apartmentType */
    };

    Windows::System::DispatcherQueueController controller{nullptr};
    check_hresult(CreateDispatcherQueueController(
        options, reinterpret_cast<abi::IDispatcherQueueController **>(put_abi(controller))));
    m_dispatcherQueueController = controller;
  }
}

void CompositionHost::CreateDesktopWindowTarget(HWND window) {
  namespace abi = ABI::Windows::UI::Composition::Desktop;

  auto interop = m_compositor.as<abi::ICompositorDesktopInterop>();
  DesktopWindowTarget target{nullptr};
  check_hresult(interop->CreateDesktopWindowTarget(
      window, false, reinterpret_cast<abi::IDesktopWindowTarget **>(put_abi(target))));
  m_target = target;
}

void CompositionHost::CreateCompositionRoot() {
  auto root = m_compositor.CreateContainerVisual();
  root.RelativeSizeAdjustment({1.0f, 1.0f});
  root.Offset({0, 0, 0});
  m_target.Root(root);
}

void CompositionHost::AddElement(float size, float x, float y) {
  if (m_target.Root()) {
    auto visuals = m_target.Root().as<ContainerVisual>().Children();
    auto visual = m_compositor.CreateSpriteVisual();

    auto element = m_compositor.CreateSpriteVisual();
    uint8_t r = (uint8_t)(double)(rand() % 255);
    ;
    uint8_t g = (uint8_t)(double)(rand() % 255);
    ;
    uint8_t b = (uint8_t)(double)(rand() % 255);
    ;

    element.Brush(m_compositor.CreateColorBrush({255, r, g, b}));
    element.Size({size, size});
    element.Offset({
        x,
        y,
        0.0f,
    });

    auto animation = m_compositor.CreateVector3KeyFrameAnimation();
    auto bottom = (float)600 - element.Size().y;
    animation.InsertKeyFrame(1, {element.Offset().x, bottom, 0});

    using timeSpan = std::chrono::duration<int, std::ratio<1, 1>>;

    std::chrono::seconds duration(2);
    std::chrono::seconds delay(3);

    animation.Duration(timeSpan(duration));
    animation.DelayTime(timeSpan(delay));
    element.StartAnimation(L"Offset", animation);
    visuals.InsertAtTop(element);

    visuals.InsertAtTop(visual);
  }
}
