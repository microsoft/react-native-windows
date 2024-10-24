/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 *
 * @flow strict-local
 * @format
 */

import * as React from 'react';
import {useLayoutEffect} from 'react';
import type {ViewProps} from './ViewPropTypes';

import View from './View';
import {findNodeHandle} from '../../ReactNative/RendererProxy';
import warnOnce from '../../Utilities/warnOnce';

/**
 * Basic View component with additional Win32 specific functionality
 */

const ViewWin32: React.AbstractComponent<
  ViewProps,
  React.ElementRef<typeof View>,
> = React.forwardRef(
  (
    {
      accessibilityLabeledBy,
      accessibilityDescribedBy,
      accessibilityControls,
      ...rest
    }: ViewProps,
    forwardedRef,
  ) => {
    /**
     * Process accessibility refs into node handles after initial DOM render, before sent across the bridge.
     * useLayoutEffect will invalidate the render to assess the ref-based accessibility properties.
     *
     * RN core moved these properties to use NativeID instead of Refs, so this is only kept for back compat
     */

    const [labeledByTarget, setLabeledByTarget] = React.useState<
      number | null | void,
    >(null);
    const [describedByTarget, setDescribedByTarget] = React.useState<
      number | null | void,
    >(null);
    const [controlsTarget, setControlsTarget] = React.useState<
      number | null | void,
    >(null);

    useLayoutEffect(() => {
      if (
        accessibilityLabeledBy !== undefined &&
        accessibilityLabeledBy.current !== null
      ) {
        warnOnce(
          'accessibilityLabeledByAsRef',
          'accessibilityLabeledBy has been deprecated.  Instead use accessibilityLabelledBy (double L), and specify it as a string matching the nativeID of the target component.  Support of specifying accessibilityLabeledBy will be removed in a future release',
        );
        setLabeledByTarget(findNodeHandle(accessibilityLabeledBy.current));
      }
    }, [accessibilityLabeledBy]);

    useLayoutEffect(() => {
      if (
        typeof accessibilityDescribedBy !== 'string' &&
        accessibilityDescribedBy !== undefined &&
        // $FlowFixMe[incompatible-use] - Using accessibilityDescribedBy as Ref for backcompat, not typed
        // $FlowFixMe[prop-missing]
        accessibilityDescribedBy.current !== null
      ) {
        warnOnce(
          'accessibilityDescribedByRef',
          'accessibilityDescribedBy should be specified as a string matching the nativeID of the target component.  Support of specifying accessibilityDescribedBy using a ref is deprecated and will be removed in a future release',
        );
        // $FlowFixMe[incompatible-use] - Using accessibilityDescribedBy as Ref for backcompat, not typed
        // $FlowFixMe[prop-missing]
        setDescribedByTarget(findNodeHandle(accessibilityDescribedBy.current));
      }
    }, [accessibilityDescribedBy]);

    useLayoutEffect(() => {
      if (
        typeof accessibilityControls !== 'string' &&
        accessibilityControls !== undefined &&
        // $FlowFixMe[incompatible-use] - Using accessibilityControls as Ref for backcompat, not typed
        // $FlowFixMe[prop-missing]
        accessibilityControls.current !== null
      ) {
        warnOnce(
          'accessibilityControlsRef',
          'accessibilityControls should be specified as a string matching the nativeID of the target component.  Support of specifying accessibilityControls using a ref is deprecated and will be removed in a future release',
        );
        // $FlowFixMe[incompatible-use] - Using accessibilityControls as Ref for backcompat, not typed
        // $FlowFixMe[prop-missing]
        setControlsTarget(findNodeHandle(accessibilityControls.current));
      }
    }, [accessibilityControls]);

    return (
      <View
        ref={forwardedRef}
        {...rest}
        {...(controlsTarget !== null
          ? {accessibilityControls: controlsTarget}
          : {})}
        {...(labeledByTarget !== null
          ? {accessibilityLabeledBy: labeledByTarget}
          : {})}
        // $FlowFixMe[exponential-spread]
        {...(describedByTarget !== null
          ? // $FlowFixMe[exponential-spread]
            {accessibilityDescribedBy: describedByTarget}
          : // $FlowFixMe[exponential-spread]
            typeof accessibilityDescribedBy === 'string'
            ? {accessibilityDescribedBy}
            : {})}
        {...(typeof accessibilityControls === 'string'
          ? {accessibilityControls}
          : {})}
      />
    );
  },
);

ViewWin32.displayName = 'ViewWin32';

module.exports = ViewWin32;
