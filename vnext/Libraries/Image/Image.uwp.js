// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//
// Portions copyright for react-native-windows:
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// NOTE: this is a shrinked copy paste of ios impl.
// TODO: provide real uwp implementation
'use strict';

const ImageResizeMode = require('ImageResizeMode');
const NativeMethodsMixin = require('NativeMethodsMixin');
const NativeModules = require('NativeModules');
const React = require('React');
const ReactNativeViewAttributes = require('ReactNativeViewAttributes');
const StyleSheet = require('StyleSheet');

const flattenStyle = require('flattenStyle');
const requireNativeComponent = require('requireNativeComponent');
const resolveAssetSource = require('resolveAssetSource');

const ImageViewManager = NativeModules.ImageViewManager;

const createReactClass = require('create-react-class');

/**
 * A React component for displaying different types of images,
 * including network images, static resources, temporary local images, and
 * images from local disk, such as the camera roll.
 *
 * This example shows both fetching and displaying an image from local
 * storage as well as one from network.
 *
 * ```ReactNativeWebPlayer
 * import React, { Component } from 'react';
 * import { AppRegistry, View, Image } from 'react-native';
 *
 * class DisplayAnImage extends Component {
 *   render() {
 *     return (
 *       <View>
 *         <Image
 *           source={require('./img/favicon.png')}
 *         />
 *         <Image
 *           style={{width: 50, height: 50}}
 *           source={{uri: 'https://facebook.github.io/react/img/logo_og.png'}}
 *         />
 *       </View>
 *     );
 *   }
 * }
 *
 * // App registration and rendering
 * AppRegistry.registerComponent('DisplayAnImage', () => DisplayAnImage);
 * ```
 *
 * You can also add `style` to an image:
 *
 * ```ReactNativeWebPlayer
 * import React, { Component } from 'react';
 * import { AppRegistry, View, Image, StyleSheet } from 'react-native';
 *
 * const styles = StyleSheet.create({
 *   stretch: {
 *     width: 50,
 *     height: 200
 *   }
 * });
 *
 * class DisplayAnImageWithStyle extends Component {
 *   render() {
 *     return (
 *       <View>
 *         <Image
 *           style={styles.stretch}
 *           source={require('./img/favicon.png')}
 *         />
 *       </View>
 *     );
 *   }
 * }
 *
 * // App registration and rendering
 * AppRegistry.registerComponent(
 *   'DisplayAnImageWithStyle',
 *   () => DisplayAnImageWithStyle
 * );
 * ```
 *
 * ### GIF and WebP support on Android
 *
 * By default, GIF and WebP are not supported on Android.
 *
 * You will need to add some optional modules in `android/app/build.gradle`, depending on the needs of your app.
 *
 * ```
 * dependencies {
 *   // If your app supports Android versions before Ice Cream Sandwich (API level 14)
 *   compile 'com.facebook.fresco:animated-base-support:0.11.0'
 *
 *   // For animated GIF support
 *   compile 'com.facebook.fresco:animated-gif:0.11.0'
 *
 *   // For WebP support, including animated WebP
 *   compile 'com.facebook.fresco:animated-webp:0.11.0'
 *   compile 'com.facebook.fresco:webpsupport:0.11.0'
 *
 *   // For WebP support, without animations
 *   compile 'com.facebook.fresco:webpsupport:0.11.0'
 * }
 * ```
 *
 * Also, if you use GIF with ProGuard, you will need to add this rule in `proguard-rules.pro` :
 * ```
 * -keep class com.facebook.imagepipeline.animated.factory.AnimatedFactoryImpl {
 *   public AnimatedFactoryImpl(com.facebook.imagepipeline.bitmaps.PlatformBitmapFactory, com.facebook.imagepipeline.core.ExecutorSupplier);
 * }
 * ```
 *
 */
// $FlowFixMe(>=0.41.0)
const Image = createReactClass({

  statics: {
    resizeMode: ImageResizeMode,
    /**
     * Retrieve the width and height (in pixels) of an image prior to displaying it.
     * This method can fail if the image cannot be found, or fails to download.
     *
     * In order to retrieve the image dimensions, the image may first need to be
     * loaded or downloaded, after which it will be cached. This means that in
     * principle you could use this method to preload images, however it is not
     * optimized for that purpose, and may in future be implemented in a way that
     * does not fully load/download the image data. A proper, supported way to
     * preload images will be provided as a separate API.
     *
     * Does not work for static image resources.
     *
     * @param uri The location of the image.
     * @param success The function that will be called if the image was successfully found and width
     * and height retrieved.
     * @param failure The function that will be called if there was an error, such as failing to
     * to retrieve the image.
     *
     * @returns void
     *
     * @platform ios
     */
    getSize: function(
      uri: string,
      success: (width: number, height: number) => void,
      failure?: (error: any) => void,
    ) {
      ImageViewManager.getSize(uri)
      .then(function(sizes) {
        success(sizes.width, sizes.height);
      })
      .catch(failure || function() {
        console.warn('Failed to get size for image: ' + uri);
      });
    },
    /**
     * Prefetches a remote image for later use by downloading it to the disk
     * cache
     *
     * @param url The remote location of the image.
     *
     * @return The prefetched image.
     */
    prefetch(url: string) {
      return ImageViewManager.prefetchImage(url);
    },
    /**
     * Resolves an asset reference into an object which has the properties `uri`, `width`,
     * and `height`. The input may either be a number (opaque type returned by
     * require('./foo.png')) or an `ImageSource` like { uri: '<http location || file path>' }
     */
    resolveAssetSource: resolveAssetSource,
  },

  mixins: [NativeMethodsMixin],

  /**
   * `NativeMethodsMixin` will look for this when invoking `setNativeProps`. We
   * make `this` look like an actual native component class.
   */
  viewConfig: {
    uiViewClassName: 'UIView',
    validAttributes: ReactNativeViewAttributes.UIView
  },

  render: function() {
    const source = resolveAssetSource(this.props.source) || { uri: undefined, width: undefined, height: undefined };

    let sources;
    let style;
    if (Array.isArray(source)) {
      style = flattenStyle([styles.base, this.props.style]) || {};
      sources = source;
    } else {
      const {width, height, uri} = source;
      style = flattenStyle([{width, height}, styles.base, this.props.style]) || {};
      sources = [source];

      if (uri === '') {
        console.warn('source.uri should not be an empty string');
      }
    }

    const resizeMode = this.props.resizeMode || (style || {}).resizeMode || 'cover'; // Workaround for flow bug t7737108
    const tintColor = (style || {}).tintColor; // Workaround for flow bug t7737108

    if (this.props.src) {
      console.warn('The <Image> component requires a `source` property rather than `src`.');
    }

    const accessibilityLabel = this.props.accessibilityLabel;

    return (
      <RCTImageView
        {...this.props}
        style={style}
        resizeMode={resizeMode}
        tintColor={tintColor}
        source={sources}
        accessibilityLabel={accessibilityLabel}
      />
    );
  },
});

const styles = StyleSheet.create({
  base: {
    overflow: 'hidden',
  },
});

const RCTImageView = requireNativeComponent('RCTImageView', Image);

module.exports = Image;
