/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

import type {ViewProps} from './ViewPropTypes';

import ViewNativeComponent from './ViewNativeComponent';
import TextAncestor from '../../Text/TextAncestor';
import * as React from 'react';
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
    // Windows]
  );
});

View.displayName = 'View';

module.exports = View;
