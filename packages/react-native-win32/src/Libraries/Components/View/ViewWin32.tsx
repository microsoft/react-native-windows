/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 */

import * as React from 'react';
import RN = require('react-native');
import { View, findNodeHandle, NativeModules } from 'react-native';
import { IViewWin32Props, UseFrom } from './ViewWin32.Props';

/**
 * Basic View component with additional Win32 specific functionality
 */

type InnerViewWin32Props = UseFrom<IViewWin32Props, RN.ViewProps, 'accessibilityRole'> &
  UseFrom<IViewWin32Props, RN.ViewProps, 'accessibilityStates'> &
  UseFrom<IViewWin32Props, RN.ViewProps, 'accessibilityActions'> &
  UseFrom<IViewWin32Props, RN.ViewProps, 'onAccessibilityAction'>;

type ViewWin32Type = React.ForwardRefExoticComponent<
IViewWin32Props & React.RefAttributes<View>
> &
  View;


export const ViewWin32 = React.forwardRef(
  (props: IViewWin32Props, ref: React.Ref<any>) => {

    /**
     * Check for raw text in the DOM.
     */
    if (__DEV__) {
      React.Children.toArray(props.children).forEach(item => {
        if (typeof item === 'string') {
          console.error(`Unexpected text node: ${item}. A text node cannot be a child of a <View>.`);
        }
      });
    }
    const [labeledByTarget, setLabeledByTarget] = React.useState(null);
    const [describedByTarget, setDescribedByTarget] = React.useState(null);
    /**
     * Process accessibility refs into node handles after initial DOM render, before sent across the bridge.
     * useLayoutEffect will invalidate the render to assess the ref-based accessibility properties.
     */
    const {accessibilityLabeledBy, accessibilityDescribedBy, ...rest} = props;
    React.useLayoutEffect(() => {
      if (accessibilityLabeledBy !== undefined && accessibilityLabeledBy?.current !== null)
      {
        setLabeledByTarget(findNodeHandle(accessibilityLabeledBy.current as
          | null
          | number
          | React.Component<any, any, any>
          | React.ComponentClass<any, any>));
      }

      if (accessibilityDescribedBy !== undefined && accessibilityDescribedBy?.current !== null)
      {
        setDescribedByTarget(findNodeHandle(accessibilityDescribedBy.current as
          | null
          | number
          | React.Component<any, any, any>
          | React.ComponentClass<any, any>));
      }
    }, [accessibilityLabeledBy, accessibilityDescribedBy]);

    /**
     * Add focus() as a callable function to the forwarded reference.
     */
    const focusRef = React.useRef<ViewWin32>();
    React.useImperativeHandle(ref, () => ({
      focus: () => {
        NativeModules.UIManager.dispatchViewManagerCommand(
          findNodeHandle(focusRef.current),
          NativeModules.UIManager.getViewManagerConfig('RCTView').Commands.focus,
          null
          );
      },
      ...focusRef.current,
    }));

    return <View ref={focusRef}
    {...(rest as InnerViewWin32Props)}
    {...((labeledByTarget !== null) ? {accessibilityLabeledBy:labeledByTarget} : {})}
    {...((describedByTarget !== null) ? {accessibilityDescribedBy:describedByTarget} : {})}
    />;
  });

export type ViewWin32 = ViewWin32Type;
