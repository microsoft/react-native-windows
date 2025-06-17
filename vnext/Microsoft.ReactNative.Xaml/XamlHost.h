#pragma once

#if defined(RNW_NEW_ARCH)

void RegisterXamlHostComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder);

void RegisterXamlControl(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder);

#endif // defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
