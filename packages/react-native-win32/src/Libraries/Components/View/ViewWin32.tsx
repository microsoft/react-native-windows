/**
 * Copyright: Microsoft 2015
 *
 * RN-specific implementation of the cross-platform View abstraction.
 */

import * as React from 'react';
import RN = require('react-native');
import { NativeModules, findNodeHandle } from 'react-native';
import { IViewWin32Props, IViewWin32, UseFrom } from './ViewWin32.Props';

/**
 * Basic View component with additional Win32 specific functionality
 */

type InnerViewProps = UseFrom<IViewWin32Props, RN.ViewProps, 'accessibilityRole'> &
  UseFrom<IViewWin32Props, RN.ViewProps, 'accessibilityStates'> &
  UseFrom<IViewWin32Props, RN.ViewProps, 'accessibilityActions'> &
  UseFrom<IViewWin32Props, RN.ViewProps, 'onAccessibilityAction'>;

export class ViewWin32 extends React.Component<IViewWin32Props, {}> implements IViewWin32 {
  constructor(props: IViewWin32Props) {
    super(props);
  }

  public render() {
    if (__DEV__) {
      React.Children.toArray(this.props.children).forEach(item => {
        if (typeof item === 'string') {
          console.error(`Unexpected text node: ${item}. A text node cannot be a child of a <View>.`);
        }
      });
    }
    return <RN.View {...(this.props as InnerViewProps)} />;
  }

  /**
   * Moves focus to this view
   */
  public focus() {
    NativeModules.UIManager.dispatchViewManagerCommand(
      findNodeHandle(this),
      NativeModules.UIManager.getViewManagerConfig('RCTView').Commands.focus,
      null
    );
  }
}
