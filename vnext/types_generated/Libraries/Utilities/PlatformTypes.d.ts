/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<69c6bac87ae207d8b6b944086a6702c7>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Utilities\PlatformTypes.js
 */

export type PlatformOSType = "ios" | "android" | "macos" | "windows" | "web" | "native";
type OptionalPlatformSelectSpec<T> = { [key in PlatformOSType]?: T };
export type PlatformSelectSpec<T> = (Omit<OptionalPlatformSelectSpec<T>, "default"> & {
  default: T;
}) | OptionalPlatformSelectSpec<T>;
type IOSPlatform = {
  /** The platform identifier. Always `'ios'` on iOS. */
  OS: "ios";
  /**
   * The OS version string (e.g. `'26.0'`).
   *
   * @platform ios
   */
  get Version(): string;
  /**
   * An object of platform-specific constants, including `reactNativeVersion`,
   * `osVersion`, `interfaceIdiom`, and more.
   *
   * @platform ios
   */
  get constants(): {
    forceTouchAvailable: boolean;
    interfaceIdiom: string;
    isTesting: boolean;
    isDisableAnimations?: boolean | undefined;
    osVersion: string;
    reactNativeVersion: {
      major: number;
      minor: number;
      patch: number;
      prerelease: string | undefined;
    };
    systemName: string;
  };
  /**
   * Whether the app is running on an iPad.
   *
   * @platform ios
   */
  get isPad(): boolean;
  /**
   * Whether the app is running on a TV device.
   */
  get isTV(): boolean;
  /**
   * Whether the app is running on Apple Vision Pro.
   *
   * @platform ios
   */
  get isVision(): boolean;
  /**
   * Whether the app is running in a test environment.
   */
  get isTesting(): boolean;
  get isDisableAnimations(): boolean;
  get isMacCatalyst(): boolean;
  /**
   * Returns the most fitting value for the current platform from the given
   * spec object. The spec can include keys for specific platform names
   * (`'ios'`, `'android'`, etc.) and a `default` fallback.
   */
  select: <T>(spec: PlatformSelectSpec<T>) => T;
};
type AndroidPlatform = {
  /** The platform identifier. Always `'android'` on Android. */
  OS: "android";
  /**
   * The Android API level (e.g. `34`).
   *
   * @platform android
   */
  get Version(): number;
  /**
   * An object of platform-specific constants, including `reactNativeVersion`,
   * `Version`, `Release`, `Model`, and more.
   *
   * @platform android
   */
  get constants(): {
    isTesting: boolean;
    isDisableAnimations?: boolean | undefined;
    reactNativeVersion: {
      major: number;
      minor: number;
      patch: number;
      prerelease: string | undefined;
    };
    Version: number;
    Release: string;
    Serial: string;
    Fingerprint: string;
    Model: string;
    ServerHost?: string | undefined;
    uiMode: string;
    Brand: string;
    Manufacturer: string;
  };
  /**
   * Whether the app is running on a TV device.
   */
  get isTV(): boolean;
  /**
   * Whether the app is running on Apple Vision Pro. Always `false` on Android.
   */
  get isVision(): boolean;
  /**
   * Whether the app is running in a test environment.
   */
  get isTesting(): boolean;
  get isDisableAnimations(): boolean;
  /**
   * Returns the most fitting value for the current platform from the given
   * spec object. The spec can include keys for specific platform names
   * (`'ios'`, `'android'`, etc.) and a `default` fallback.
   */
  select: <T>(spec: PlatformSelectSpec<T>) => T;
};
type WindowsPlatform = {
  OS: "windows";
  get Version(): number;
  get constants(): {
    isTesting: boolean;
    isDisableAnimations?: boolean | undefined;
    reactNativeVersion: {
      major: number;
      minor: number;
      patch: number;
      prerelease: string | undefined;
    };
    reactNativeWindowsVersion: {
      major: number;
      minor: number;
      patch: number;
    };
    osVersion: number;
  };
  get isTesting(): boolean;
  get isDisableAnimations(): boolean;
  get isTV(): boolean;
  select: <T>(spec: PlatformSelectSpec<T>) => T;
};
type MacOSPlatform = {
  OS: "macos";
  get Version(): string;
  get constants(): {
    isTesting: boolean;
    osVersion: string;
    reactNativeVersion: {
      major: number;
      minor: number;
      patch: number;
      prerelease: number | undefined;
    };
    systemName: string;
  };
  get isTV(): boolean;
  get isVision(): boolean;
  get isTesting(): boolean;
  get isDisableAnimations(): boolean;
  select: <T>(spec: PlatformSelectSpec<T>) => T;
};
type WebPlatform = {
  OS: "web";
  get Version(): string;
  get constants(): {
    reactNativeVersion: {
      major: number;
      minor: number;
      patch: number;
      prerelease: string | undefined;
    };
  };
  get isTV(): boolean;
  get isTesting(): boolean;
  get isDisableAnimations(): boolean;
  select: <T>(spec: PlatformSelectSpec<T>) => T;
};
export type PlatformType = IOSPlatform | AndroidPlatform | WindowsPlatform | MacOSPlatform | WebPlatform;
