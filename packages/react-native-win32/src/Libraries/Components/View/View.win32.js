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
import invariant from 'invariant'; // [Windows]

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
  // [Win32
  invariant(
    // $FlowFixMe Wanting to catch untyped usages
    props.acceptsKeyboardFocus === undefined,
    'Support for the "acceptsKeyboardFocus" property has been removed in favor of "focusable"',
  );
  // Win32]

  return (
    // [Windows
    // In core this is a TextAncestor.Provider value={false} See
    // https://github.com/facebook/react-native/commit/66601e755fcad10698e61d20878d52194ad0e90c
    // But since Views are not currently supported in Text, we do not need the extra provider
    <TextAncestor.Consumer>
      {hasTextAncestor => {
        invariant(
          !hasTextAncestor,
          'Nesting of <View> within <Text> is not currently supported.',
        );

        return <ViewNativeComponent {...props} ref={forwardedRef} />;
      }}
    </TextAncestor.Consumer>
  );
});

View.displayName = 'View';

module.exports = View;
