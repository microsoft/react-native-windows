/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

import type {ViewProps} from './ViewPropTypes';

const React = require('react');
import ViewNativeComponent from './ViewNativeComponent';
const TextAncestor = require('../../Text/TextAncestor');
import warnOnce from '../../Utilities/warnOnce'; // [Windows]

export type Props = ViewProps;

/**
 * The most fundamental component for building a UI, View is a container that
 * supports layout with flexbox, style, some touch handling, and accessibility
 * controls.
 *
 * @see https://reactnative.dev/docs/view.html
 */
const View: React.AbstractComponent<
  ViewProps,
  React.ElementRef<typeof ViewNativeComponent>,
> = React.forwardRef((props: ViewProps, forwardedRef) => {
  // [Windows Intercept props to warn about them going away
  if (props.acceptsKeyboardFocus !== undefined) {
    warnOnce(
      'deprecated-acceptsKeyboardFocus',
      '"acceptsKeyboardFocus" has been deprecated in favor of "focusable" and will be removed in React Native for Windows 0.64',
    );
  }
  // Windows]

  return (
    <TextAncestor.Provider value={false}>
      <ViewNativeComponent {...props} ref={forwardedRef} />
    </TextAncestor.Provider>
  );
});

View.displayName = 'View';

module.exports = View;
