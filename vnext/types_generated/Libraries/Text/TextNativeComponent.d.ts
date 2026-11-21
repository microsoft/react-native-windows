/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8398d783d0f9df3f49866aedda5a6277>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Text\TextNativeComponent.js
 */

import type { HostComponent } from "../../src/private/types/HostComponent";
import type { ProcessedColorValue } from "../StyleSheet/processColor";
import type { GestureResponderEvent } from "../Types/CoreEventTypes";
import type { TextProps } from "./TextProps";
export type NativeTextProps = Readonly<Omit<TextProps, "isHighlighted" | "selectionColor" | "onClick" | "isPressable"> & {
  isHighlighted?: boolean | undefined;
  selectionColor?: ProcessedColorValue | undefined;
  onClick?: ((event: GestureResponderEvent) => unknown) | undefined;
  isPressable?: boolean | undefined;
}>;
/**
 * `NativeText` is an internal React Native host component, and is exported to
 * provide lower-level access for libraries.
 *
 * @warning `<unstable_NativeText>` provides no semver guarantees and is not
 *   intended to be used in app code. Please use
 *   [`<Text>`](https://reactnative.dev/docs/text) instead.
 */
export declare const NativeText: HostComponent<NativeTextProps>;
export declare type NativeText = typeof NativeText;
export declare const NativeVirtualText: HostComponent<NativeTextProps>;
export declare type NativeVirtualText = typeof NativeVirtualText;
export declare const NativeSelectableText: HostComponent<NativeTextProps>;
export declare type NativeSelectableText = typeof NativeSelectableText;
