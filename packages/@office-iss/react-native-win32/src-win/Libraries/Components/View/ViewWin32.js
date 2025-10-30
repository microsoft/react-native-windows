/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 *
 * @flow strict-local
 * @format
 */

import * as React from 'react';
import type {ViewProps} from './ViewPropTypes';

import View from './View';
import {findNodeHandle} from '../../ReactNative/RendererProxy';
import warnOnce from '../../Utilities/warnOnce';

/**
 * Basic View component with additional Win32 specific functionality
 */

type ViewWin32State = {
  labeledByTarget: number | null | void,
  describedByTarget: number | null | void,
  controlsTarget: number | null | void,
};

class ViewWin32Internal extends React.Component<ViewProps, ViewWin32State> {
  constructor(props: ViewProps) {
    super(props);
    this.state = {
      labeledByTarget: null,
      describedByTarget: null,
      controlsTarget: null,
    };
  }

  componentDidMount() {
    this.updateAccessibilityTargets();
  }

  componentDidUpdate(prevProps: ViewProps) {
    if (
      prevProps.accessibilityLabeledBy !== this.props.accessibilityLabeledBy ||
      prevProps.accessibilityDescribedBy !==
        this.props.accessibilityDescribedBy ||
      prevProps.accessibilityControls !== this.props.accessibilityControls
    ) {
      this.updateAccessibilityTargets();
    }
  }

  updateAccessibilityTargets = () => {
    const {
      accessibilityLabeledBy,
      accessibilityDescribedBy,
      accessibilityControls,
    } = this.props;

    /**
     * Process accessibility refs into node handles after initial DOM render, before sent across the bridge.
     * This logic will update the state to assess the ref-based accessibility properties.
     *
     * RN core moved these properties to use NativeID instead of Refs, so this is only kept for back compat
     */

    if (
      accessibilityLabeledBy !== undefined &&
      accessibilityLabeledBy.current !== null
    ) {
      warnOnce(
        'accessibilityLabeledByAsRef',
        'accessibilityLabeledBy has been deprecated.  Instead use accessibilityLabelledBy (double L), and specify it as a string matching the nativeID of the target component.  Support of specifying accessibilityLabeledBy will be removed in a future release',
      );
      this.setState({
        labeledByTarget: findNodeHandle(accessibilityLabeledBy.current),
      });
    }

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
      this.setState({
        describedByTarget: findNodeHandle(accessibilityDescribedBy.current),
      });
    }

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
      this.setState({
        controlsTarget: findNodeHandle(accessibilityControls.current),
      });
    }
  };

  render() {
    const {
      accessibilityLabeledBy,
      accessibilityDescribedBy,
      accessibilityControls,
      ...rest
    } = this.props;
    const {labeledByTarget, describedByTarget, controlsTarget} = this.state;

    return (
      <View
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
  }
}

// $FlowFixMe[prop-missing]
const ViewWin32: React.AbstractComponent<
  ViewProps,
  React.ElementRef<typeof View>,
> = React.forwardRef<ViewProps, typeof View>((props: ViewProps, ref) => (
  <ViewWin32Internal {...props} />
));

ViewWin32.displayName = 'ViewWin32';

module.exports = ViewWin32;
