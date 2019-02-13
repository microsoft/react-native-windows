// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
/**
 * @providesModule Image
 * @flow
 */
// NOTE: this is a shrinked copy paste of ios impl.
// TODO: provide real uwp implementation
'use strict';

const EdgeInsetsPropType = require('EdgeInsetsPropType');
const ImageResizeMode = require('ImageResizeMode');
const ImageSourcePropType = require('ImageSourcePropType');
const ImageStylePropTypes = require('ImageStylePropTypes');
const NativeMethodsMixin = require('NativeMethodsMixin');
const NativeModules = require('NativeModules');
const React = require('React');
const ReactNativeViewAttributes = require('ReactNativeViewAttributes');
const StyleSheet = require('StyleSheet');
const StyleSheetPropType = require('StyleSheetPropType');

const flattenStyle = require('flattenStyle');
const requireNativeComponent = require('requireNativeComponent');
const resolveAssetSource = require('resolveAssetSource');

const PropTypes = require('prop-types');

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
  propTypes: {
    /**
     * > `ImageResizeMode` is an `Enum` for different image resizing modes, set via the
     * > `resizeMode` style property on `Image` components. The values are `contain`, `cover`,
     * > `stretch`, `center`, `repeat`.
     */
    style: StyleSheetPropType(ImageStylePropTypes),
    /**
     * The image source (either a remote URL or a local file resource).
     *
     * This prop can also contain several remote URLs, specified together with
     * their width and height and potentially with scale/other URI arguments.
     * The native side will then choose the best `uri` to display based on the
     * measured size of the image container. A `cache` property can be added to
     * control how networked request interacts with the local cache.
     */
    source: ImageSourcePropType,
    /**
     * Determines how to resize the image when the frame doesn't match the raw
     * image dimensions.
     *
     * - `cover`: Scale the image uniformly (maintain the image's aspect ratio)
     * so that both dimensions (width and height) of the image will be equal
     * to or larger than the corresponding dimension of the view (minus padding).
     *
     * - `contain`: Scale the image uniformly (maintain the image's aspect ratio)
     * so that both dimensions (width and height) of the image will be equal to
     * or less than the corresponding dimension of the view (minus padding).
     *
     * - `stretch`: Scale width and height independently, This may change the
     * aspect ratio of the src.
     *
     * - `repeat`: Repeat the image to cover the frame of the view. The
     * image will keep it's size and aspect ratio. (iOS only)
     */
    resizeMode: PropTypes.oneOf(['cover', 'contain', 'stretch', 'repeat', 'center']),
      /**
     * The text that's read by the screen reader when the user interacts with
     * the image.
     * @platform ios
     */
    accessibilityLabel: PropTypes.string,
    /**
     * A unique identifier for this element to be used in UI Automation
     * testing scripts.
     */
    testID: PropTypes.string,
    /**
     * Invoked on mount and layout changes with
     * `{nativeEvent: {layout: {x, y, width, height}}}`.
     */
    onLayout: PropTypes.func,
    /**
     * Invoked on load start.
     *
     * e.g., `onLoadStart={(e) => this.setState({loading: true})}`
     */
    onLoadStart: PropTypes.func,
    /**
     * Invoked on load error with `{nativeEvent: {error}}`.
     */
    onError: PropTypes.func,
    /**
     * Invoked when load completes successfully.
     */
    onLoad: PropTypes.func,
    /**
     * Invoked when load either succeeds or fails.
     */
    onLoadEnd: PropTypes.func,
  },

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
