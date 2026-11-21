/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<629fc9248ef23410c2d7583789930590>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\ProgressBarAndroid\ProgressBarAndroid.js
 */

import * as React from "react";
import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ProgressBarAndroidProps } from "./ProgressBarAndroidTypes";
export type ProgressBarAndroidInstance = HostInstance;
export type { ProgressBarAndroidProps };
type Omit<T, K> = T extends any ? Pick<T, Exclude<keyof T, K>> : T;
declare let ProgressBarAndroid: (props: Omit<Omit<ProgressBarAndroidProps, never>, "ref"> & {
  ref?: React.Ref<ProgressBarAndroidInstance> | undefined;
}) => React.ReactNode;
/**
 * ProgressBarAndroid has been extracted from react-native core and will be removed in a future release.
 * It can now be installed and imported from `@react-native-community/progress-bar-android` instead of 'react-native'.
 * @see https://github.com/react-native-community/progress-bar-android
 * @deprecated
 */
declare const $$ProgressBarAndroid: typeof ProgressBarAndroid;
declare type $$ProgressBarAndroid = typeof $$ProgressBarAndroid;
export default $$ProgressBarAndroid;
