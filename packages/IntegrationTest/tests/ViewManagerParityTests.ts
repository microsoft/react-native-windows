/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import {functionTest} from './lib/TestDefinition';
import {UIManager} from 'react-native';

const LayoutPropTypes = require('react-native/Libraries/DeprecatedPropTypes/DeprecatedLayoutPropTypes');
const ReactNativeViewConfigRegistry = require('react-native/Libraries/Renderer/shims/ReactNativeViewConfigRegistry');

const layoutProps = Object.keys(LayoutPropTypes);

function diffViewManagerViewConfig(opts: {
  component: string;
  ignoredProps: string[];
  notYetImplementedProps?: Array<[string, number]>;
}) {
  const viewManagerConfig = UIManager.getViewManagerConfig(opts.component);
  const viewConfig = ReactNativeViewConfigRegistry.get(opts.component);

  const viewManagerConfigProps: string[] = Object.keys(
    // @ts-ignore
    viewManagerConfig.NativeProps,
  );

  const unsupportedProps = _.difference(
    Object.keys(viewConfig.validAttributes),
    layoutProps,
    opts.ignoredProps,
    viewManagerConfigProps,
    (opts.notYetImplementedProps || []).map(p => p[0]),
  );

  if (unsupportedProps.length > 0) {
    throw new Error(
      `The view manager for the native component "${
        opts.component
      }" is missing support for properties: ${unsupportedProps
        .map(p => `\n - ${p}`)
        .join('')}`,
    );
  }

  const falseNyiProps = _.difference(
    viewManagerConfigProps,
    (opts.notYetImplementedProps || []).map(p => p[0]),
  );

  if (falseNyiProps.length > 0) {
    throw new Error(
      `The following props for ${
        opts.component
      } are marked as not yet implemented but an implementation was found: ${falseNyiProps
        .map(p => `\n - ${p}`)
        .join('')}`,
    );
  }
}

functionTest('RCTView', () =>
  diffViewManagerViewConfig({
    component: 'RCTView',
    ignoredProps: [
      'hasTVPreferredFocus', // TV only
      'nativeBackgroundAndroid', // Android only
      'nativeForegroundAndroid', // Android only
      'nextFocusDown', // TV only
      'nextFocusForward', // TV only
      'nextFocusLeft', // TV only
      'nextFocusRight', // TV only
      'nextFocusUp', // TV only
      'accessibilityElementsHidden', // iOS only
      'accessibilityIgnoresInvertColors', // iOS only
      'accessibilityStates', // Removed in older versions of RN
      'accessibilityViewIsModal', // iOS only
      'clickable', // Android-specific legacy
      'elevation', // Android only
      'renderToHardwareTextureAndroid', // Android only
      'rotation', // Android legacy
      'scaleX', // Android legacy
      'scaleY', // Android legacy
      'shouldRasterizeIOS', // iOS Only
      'translateX', // Android legacy
      'translateY', // Android legacy
    ],
    notYetImplementedProps: [
      ['backfaceVisibility', 3538],
      ['hitSlop', 5341],
      ['borderBottomColor', 3077],
      ['borderEndColor', 3077],
      ['borderLeftColor', 3077],
      ['borderRightColor', 3077],
      ['borderStartColor', 3077],
      ['borderStyle', 3078],
      ['borderTopColor', 3077],
      ['collapsable', 5800],
      ['importantForAccessibility', 5113],
      ['nativeID', 6096],
      ['needsOffscreenAlphaCompositing', 6097],
      ['onAccessibilityAction', 3123],
      ['onAccessibilityEscape', 3119],
      ['onAccessibilityTap', 3120],
      ['onMagicTap', 3121],
      ['removeClippedSubviews', 3067],
      ['shadowColor', 2800],
      ['shadowOffset', 2800],
      ['shadowOpacity', 2800],
      ['shadowRadius', 2800],
    ],
  }),
);
