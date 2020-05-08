/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 */

import * as React from 'react';
import RN = require('react-native');
import { View } from 'react-native';
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

    /**
     *
     */

    /**
     * Add focus() as a callable function.  PROBLEM: findNodeHandle doesn't work, focusRef.current is undefined ...
     */
    // const focusRef = React.useRef<ViewWin32>();
    // React.useImperativeHandle(ref, () => ({
    //   focus: () => {
    //     NativeModules.UIManager.dispatchViewManagerCommand(
    //       findNodeHandle(focusRef.current),
    //       NativeModules.UIManager.getViewManagerConfig('RCTView').Commands.focus,
    //       null
    //       );
    //   },
    // }));

    return <View ref={ref} {...(props as InnerViewWin32Props)} />;
  });

export type ViewWin32 = ViewWin32Type;
