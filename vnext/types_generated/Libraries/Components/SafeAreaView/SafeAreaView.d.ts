/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8ac101edb7631ebe66b8d4c61b5cb194>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\SafeAreaView\SafeAreaView.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ViewProps } from "../View/ViewPropTypes";
import * as React from "react";
export type SafeAreaViewInstance = HostInstance;
declare const SafeAreaView: (props: Omit<ViewProps, "ref"> & {
  ref?: React.Ref<SafeAreaViewInstance> | undefined;
}) => React.ReactNode;
/**
 * Renders content within the safe area boundaries of a device. Currently only applicable to iOS devices with iOS version 11 or later. Automatically applies padding to reflect the portion of the view not covered by navigation bars, tab bars, toolbars, and other ancestor views.
 *
 * @see https://reactnative.dev/docs/safeareaview
 * @deprecated Use `react-native-safe-area-context` instead.
 * @platform ios
 */
declare const $$SafeAreaView: typeof SafeAreaView;
declare type $$SafeAreaView = typeof $$SafeAreaView;
export default $$SafeAreaView;
