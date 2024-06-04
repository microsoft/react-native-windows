/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {RootTag} from '../Types/RootTagTypes';
import type {AbstractImageIOS, ImageIOS} from './ImageTypes.flow';
import TextAncestor from '../Text/TextAncestor'; // [Windows]
import invariant from 'invariant'; // [Windows]

import type {ImageProps as ImagePropsType} from './ImageProps';

import type {ImageStyle, ImageStyleProp} from '../StyleSheet/StyleSheet';
import NativeImageLoaderWin32 from './NativeImageLoaderWin32'; // [Win32] Replace iOS

import {createRootTag} from '../ReactNative/RootTag';
import flattenStyle from '../StyleSheet/flattenStyle';
import StyleSheet from '../StyleSheet/StyleSheet';
import ImageAnalyticsTagContext from './ImageAnalyticsTagContext';
import {
  unstable_getImageComponentDecorator,
  useWrapRefWithImageAttachedCallbacks,
} from './ImageInjection';
import {getImageSourcesFromImageProps} from './ImageSourceUtils';
import {convertObjectFitToResizeMode} from './ImageUtils';
import ImageViewNativeComponent from './ImageViewNativeComponent';
import resolveAssetSource from './resolveAssetSource';
import * as React from 'react';

function getSize(
  uri: string,
  success?: (width: number, height: number) => void,
  // $FlowFixMe[unclear-type]
  failure?: (error: any) => void,
) {
  //[Win32
  /*
  NativeNativeImageLoaderWin32IOS.getSize(uri)
    .then(([width, height]) => success(width, height))
    .catch(
      failure ||
        function () {
          console.warn('Failed to get size for image: ' + uri);
        },
    );
  */

  NativeImageLoaderWin32.getSize(
    uri,
    (width: number, height: number, err?: string) => {
      // $FlowFixMe[sketchy-null-string]
      if (!err) {
        // $FlowFixMe[not-a-function]
        success(width, height);
      } else {
        if (failure) {
          failure(err);
        } else {
          console.warn('Failure to get size for image: ' + uri);
        }
      }
    },
  );
  // ]Win32
}

function getSizeWithHeaders(
  uri: string,
  headers: {[string]: string, ...},
  success?: (width: number, height: number) => void,
  failure?: (error: mixed) => void,
  // $FlowFixMe[unclear-type]
): any {
  return NativeImageLoaderWin32.getSizeWithHeaders(uri, headers)
    .then(function (sizes) {
      // $FlowFixMe[not-a-function]
      success(sizes.width, sizes.height);
    })
    .catch(
      failure ||
        function () {
          console.warn('Failed to get size for image: ' + uri);
        },
    );
}

function prefetchWithMetadata(
  url: string,
  queryRootName: string,
  rootTag?: ?RootTag,
): Promise<boolean> {
  if (NativeImageLoaderWin32.prefetchImageWithMetadata) {
    // number params like rootTag cannot be nullable before TurboModules is available
    return NativeImageLoaderWin32.prefetchImageWithMetadata(
      url,
      queryRootName,
      // NOTE: RootTag type
      rootTag != null ? rootTag : createRootTag(0),
    );
  } else {
    return NativeImageLoaderWin32.prefetchImage(url);
  }
}

function prefetch(url: string): Promise<boolean> {
  return NativeImageLoaderWin32.prefetchImage(url);
}

async function queryCache(
  urls: Array<string>,
): Promise<{[string]: 'memory' | 'disk' | 'disk/memory', ...}> {
  return NativeImageLoaderWin32.queryCache(urls);
}

/**
 * A React component for displaying different types of images,
 * including network images, static resources, temporary local images, and
 * images from local disk, such as the camera roll.
 *
 * See https://reactnative.dev/docs/image
 */
let BaseImage: AbstractImageIOS = React.forwardRef((props, forwardedRef) => {
  const source = getImageSourcesFromImageProps(props) || {
    uri: undefined,
    width: undefined,
    height: undefined,
  };

  let sources;
  let style: ImageStyle;

  if (Array.isArray(source)) {
    style =
      flattenStyle<ImageStyleProp>([styles.base, props.style]) ||
      ({}: ImageStyle);
    sources = source;
  } else {
    const {uri} = source;
    const width = source.width ?? props.width;
    const height = source.height ?? props.height;
    style =
      flattenStyle<ImageStyleProp>([
        {width, height},
        styles.base,
        props.style,
      ]) || ({}: ImageStyle);
    sources = [source];

    if (uri === '') {
      console.warn('source.uri should not be an empty string');
    }
  }

  const objectFit =
    style.objectFit != null
      ? convertObjectFitToResizeMode(style.objectFit)
      : null;
  const resizeMode =
    objectFit || props.resizeMode || style.resizeMode || 'cover';
  const tintColor = props.tintColor ?? style.tintColor;

  if (props.children != null) {
    throw new Error(
      'The <Image> component cannot contain children. If you want to render content on top of the image, consider using the <ImageBackground> component or absolute positioning.',
    );
  }
  const {
    'aria-busy': ariaBusy,
    'aria-checked': ariaChecked,
    'aria-disabled': ariaDisabled,
    'aria-expanded': ariaExpanded,
    'aria-multiselectable': ariaMultiselectable, // Win32
    'aria-required': ariaRequired, // Win32
    'aria-selected': ariaSelected,
    height,
    src,
    width,
    ...restProps
  } = props;

  const _accessibilityState = {
    busy: ariaBusy ?? props.accessibilityState?.busy,
    checked: ariaChecked ?? props.accessibilityState?.checked,
    disabled: ariaDisabled ?? props.accessibilityState?.disabled,
    expanded: ariaExpanded ?? props.accessibilityState?.expanded,
    multiselectable:
      ariaMultiselectable ?? props.accessibilityState?.multiselectable, // Win32
    required: ariaRequired ?? props.accessibilityState?.required, // Win32
    selected: ariaSelected ?? props.accessibilityState?.selected,
  };
  const accessibilityLabel = props['aria-label'] ?? props.accessibilityLabel;

  const actualRef = useWrapRefWithImageAttachedCallbacks(forwardedRef);

  return (
    // [Win32
    <TextAncestor.Consumer>
      {hasTextAncestor => {
        invariant(
          !hasTextAncestor,
          'Nesting of <Image> within <Text> is not currently supported.',
        );
        // Win32]

        return (
          <ImageAnalyticsTagContext.Consumer>
            {analyticTag => {
              return (
                <ImageViewNativeComponent
                  accessibilityState={_accessibilityState}
                  {...restProps}
                  accessible={props.alt !== undefined ? true : props.accessible}
                  accessibilityLabel={accessibilityLabel ?? props.alt}
                  ref={actualRef}
                  style={style}
                  resizeMode={resizeMode}
                  tintColor={tintColor}
                  source={sources}
                  internal_analyticTag={analyticTag}
                />
              );
            }}
          </ImageAnalyticsTagContext.Consumer>
        );
      }}
    </TextAncestor.Consumer>
  );
});

const imageComponentDecorator = unstable_getImageComponentDecorator();
if (imageComponentDecorator != null) {
  BaseImage = imageComponentDecorator(BaseImage);
}

// $FlowExpectedError[incompatible-type] Eventually we need to move these functions from statics of the component to exports in the module.
const Image: ImageIOS = BaseImage;

Image.displayName = 'Image';

/**
 * Retrieve the width and height (in pixels) of an image prior to displaying it.
 *
 * See https://reactnative.dev/docs/image#getsize
 */
// $FlowFixMe[incompatible-use] This property isn't writable but we're actually defining it here for the first time.
Image.getSize = getSize;

/**
 * Retrieve the width and height (in pixels) of an image prior to displaying it
 * with the ability to provide the headers for the request.
 *
 * See https://reactnative.dev/docs/image#getsizewithheaders
 */
// $FlowFixMe[incompatible-use] This property isn't writable but we're actually defining it here for the first time.
Image.getSizeWithHeaders = getSizeWithHeaders;

/**
 * Prefetches a remote image for later use by downloading it to the disk
 * cache.
 *
 * See https://reactnative.dev/docs/image#prefetch
 */
// $FlowFixMe[incompatible-use] This property isn't writable but we're actually defining it here for the first time.
Image.prefetch = prefetch;

/**
 * Prefetches a remote image for later use by downloading it to the disk
 * cache, and adds metadata for queryRootName and rootTag.
 *
 * See https://reactnative.dev/docs/image#prefetch
 */
// $FlowFixMe[incompatible-use] This property isn't writable but we're actually defining it here for the first time.
Image.prefetchWithMetadata = prefetchWithMetadata;

/**
 * Performs cache interrogation.
 *
 *  See https://reactnative.dev/docs/image#querycache
 */
// $FlowFixMe[incompatible-use] This property isn't writable but we're actually defining it here for the first time.
Image.queryCache = queryCache;

/**
 * Resolves an asset reference into an object.
 *
 * See https://reactnative.dev/docs/image#resolveassetsource
 */
// $FlowFixMe[incompatible-use] This property isn't writable but we're actually defining it here for the first time.
Image.resolveAssetSource = resolveAssetSource;

const styles = StyleSheet.create({
  base: {
    overflow: 'hidden',
  },
});

module.exports = Image;
