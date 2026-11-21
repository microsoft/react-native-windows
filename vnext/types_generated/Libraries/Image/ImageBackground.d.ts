/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<cd011760f4acfa064e9d806354062cae>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Image\ImageBackground.js
 */

import type { ImageBackgroundProps } from "./ImageProps";
import * as React from "react";
export type { ImageBackgroundProps } from "./ImageProps";
/**
 * A common way to set a background image, similar to `background-image` in
 * CSS. Accepts the same props as `Image` and allows adding children that
 * layer on top of the background image. You must specify `width` and `height`
 * style attributes.
 *
 * Example:
 *
 * ```tsx
 * import React from 'react';
 * import {ImageBackground, StyleSheet, Text} from 'react-native';
 *
 * const App = () => (
 *   <ImageBackground
 *     style={styles.background}
 *     source={{uri: 'https://reactnative.dev/img/opengraph.png'}}>
 *     <Text>React</Text>
 *   </ImageBackground>
 * );
 *
 * const styles = StyleSheet.create({
 *   background: {
 *     width: 200,
 *     height: 200,
 *   },
 * });
 * ```
 *
 * ImageBackground is deprecated and will be removed in a future release.
 * Use a `View` with an absolutely positioned `Image` instead.
 *
 * @see https://reactnative.dev/docs/imagebackground
 * @deprecated
 */
declare class ImageBackground extends React.Component<ImageBackgroundProps> {
  setNativeProps(props: {}): void;
  render(): React.ReactNode;
}
export type ImageBackgroundInstance = ImageBackground;
export default ImageBackground;
