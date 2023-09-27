/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict
 * @format
 */

export type PlatformSelectSpec<T> = {
  default?: T,
  native?: T,
  ios?: T,
  android?: T,
  win32?: T,
  ...
};

type IOSPlatform = {
  __constants: null,
  OS: $TEMPORARY$string<'ios'>,
  // $FlowFixMe[unsafe-getters-setters]
  get Version(): string,
  // $FlowFixMe[unsafe-getters-setters]
  get constants(): {|
    forceTouchAvailable: boolean,
    interfaceIdiom: string,
    isTesting: boolean,
    isDisableAnimations?: boolean,
    osVersion: string,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?number,
    |},
    systemName: string,
  |},
  // $FlowFixMe[unsafe-getters-setters]
  get isPad(): boolean,
  // $FlowFixMe[unsafe-getters-setters]
  get isTV(): boolean,
  // $FlowFixMe[unsafe-getters-setters]
  get isTesting(): boolean,
  // $FlowFixMe[unsafe-getters-setters]
  get isDisableAnimations(): boolean,
  select: <T>(spec: PlatformSelectSpec<T>) => T,
};

type AndroidPlatform = {
  __constants: null,
  OS: $TEMPORARY$string<'android'>,
  // $FlowFixMe[unsafe-getters-setters]
  get Version(): number,
  // $FlowFixMe[unsafe-getters-setters]
  get constants(): {|
    isTesting: boolean,
    isDisableAnimations?: boolean,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?number,
    |},
    Version: number,
    Release: string,
    Serial: string,
    Fingerprint: string,
    Model: string,
    ServerHost?: string,
    uiMode: string,
    Brand: string,
    Manufacturer: string,
  |},
  // $FlowFixMe[unsafe-getters-setters]
  get isTV(): boolean,
  // $FlowFixMe[unsafe-getters-setters]
  get isTesting(): boolean,
  // $FlowFixMe[unsafe-getters-setters]
  get isDisableAnimations(): boolean,
  select: <T>(spec: PlatformSelectSpec<T>) => T,
};

type Win32Platform = {
  __constants: null,
  OS: $TEMPORARY$string<'win32'>,
  // $FlowFixMe[unsafe-getters-setters]
  get Version(): number,
  // $FlowFixMe[unsafe-getters-setters]
  get constants(): {|
    isTesting: boolean,
    isDisableAnimations?: boolean,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?string | number,
    |},
    osVersion: number,
  |},
  // $FlowFixMe[unsafe-getters-setters]
  get isTV(): boolean,
  // $FlowFixMe[unsafe-getters-setters]
  get isTesting(): boolean,
  // $FlowFixMe[unsafe-getters-setters]
  get isDisableAnimations(): boolean,
  select: <T>(spec: PlatformSelectSpec<T>) => T,
};

export type Platform = IOSPlatform | AndroidPlatform | Win32Platform;
