/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

import RCTDeviceEventEmitter from '../EventEmitter/RCTDeviceEventEmitter';
import EventEmitter, {
  type EventSubscription,
} from '../vendor/emitter/EventEmitter';
import NativeDeviceInfo, {
  type DimensionsPayload,
  type DisplayMetrics,
  type DisplayMetricsAndroid,
} from './NativeDeviceInfo';
import invariant from 'invariant';

export type {DimensionsPayload, DisplayMetrics, DisplayMetricsAndroid};

/** @deprecated Use DisplayMetrics */
export type ScaledSize = DisplayMetrics;

const eventEmitter = new EventEmitter<{
  change: [DimensionsPayload],
}>();
let dimensionsInitialized = false;
let dimensions: DimensionsPayload;

/**
 * Provides the application window's width and height. Prefer
 * `useWindowDimensions` in React components.
 *
 * @see https://reactnative.dev/docs/dimensions
 *
 * [Win32] While a global Dimensions object for window and screen dimensions is too simple for Win32,
 * attached to this object is also fontScale which is a system global value.  We expose this value
 * for large text scaling support while leaving other window dimension information undefined. These undefined
 * values will cause rendering issues if used but should avoid runtime failures in JS.
 */
class Dimensions {
  /**
   * Returns the current dimensions for `'window'` or `'screen'`. On Android,
   * `'window'` dimensions exclude the status bar and navigation bar.
   *
   * NOTE: `useWindowDimensions` is the preferred API for React components.
   *
   * Although dimensions are available immediately, they may change (e.g. due to
   * device rotation) so any rendering logic or styles that depend on these
   * constants should try to call this function on every render, rather than
   * caching the value.
   *
   * Example: `const {height, width} = Dimensions.get('window');`
   *
   * @param {string} dim Name of dimension as defined when calling `set`.
   * @returns {DisplayMetrics? | DisplayMetricsAndroid?} Value for the dimension.
   */
  static get(dim: string): DisplayMetrics | DisplayMetricsAndroid {
    // $FlowFixMe[invalid-computed-prop]
    invariant(dimensions[dim], 'No dimension set for key ' + dim);
    return dimensions[dim];
  }

  /**
   * This should only be called from native code by sending the
   * didUpdateDimensions event.
   *
   * @param {DimensionsPayload} dims Simple string-keyed object of dimensions to set
   */
  static set(dims: Readonly<DimensionsPayload>): void {
    let {screen, window} = dims;
    const {windowPhysicalPixels} = dims;
    if (windowPhysicalPixels) {
      window = {
        width: windowPhysicalPixels.width,
        height: windowPhysicalPixels.height,
        scale: windowPhysicalPixels.scale,
        fontScale: windowPhysicalPixels.fontScale,
      };
    }
    const {screenPhysicalPixels} = dims;
    if (screenPhysicalPixels) {
      screen = {
        width: screenPhysicalPixels.width,
        height: screenPhysicalPixels.height,
        scale: screenPhysicalPixels.scale,
        fontScale: screenPhysicalPixels.fontScale,
      };
    } else if (screen == null) {
      screen = window;
    }

    dimensions = {window, screen};
    if (dimensionsInitialized) {
      // Don't fire 'change' the first time the dimensions are set.
      eventEmitter.emit('change', dimensions);
    } else {
      dimensionsInitialized = true;
    }
  }

  /**
   * Add an event handler. Supported events:
   *
   * - `change`: Fires when a property within the `Dimensions` object changes,
   *   such as on device rotation or foldable device state changes. The argument
   *   to the event handler is a `DimensionsPayload` object with `window` and
   *   `screen` properties whose values are the same as the return values of
   *   `Dimensions.get('window')` and `Dimensions.get('screen')`, respectively.
   */
  static addEventListener(
    type: 'change',
    handler: Function,
  ): EventSubscription {
    invariant(
      type === 'change',
      'Trying to subscribe to unknown event: "%s"',
      type,
    );
    return eventEmitter.addListener(type, handler);
  }
}

// Subscribe before calling getConstants to make sure we don't miss any updates in between.
RCTDeviceEventEmitter.addListener(
  'didUpdateDimensions',
  (update: DimensionsPayload) => {
    Dimensions.set(update);
  },
);
Dimensions.set(NativeDeviceInfo.getConstants().Dimensions);

export default Dimensions;
