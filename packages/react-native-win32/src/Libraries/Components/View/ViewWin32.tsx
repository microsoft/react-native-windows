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

type ViewWin32State = {
  describedByTarget?: number | null;
  describedByTargetRef?: React.RefObject<any>;
  labeledByTarget?: number | null;
  labeledByTargetRef?: React.RefObject<any>;
};

export class ViewWin32 extends React.Component<IViewWin32Props, ViewWin32State> implements IViewWin32 {
  constructor(props: IViewWin32Props) {
    super(props);
    this.state = {
      describedByTarget: null,
      describedByTargetRef: null,
      labeledByTarget: null,
      labeledByTargetRef: null,
    };
  }

  public ensureRefStateIsResolved() {
    let newLabeledByTarget = null;
    let newDescribedByTarget = null;
    const currentLabeledByRef = this.state.labeledByTargetRef?.current;
    if (currentLabeledByRef !== null && currentLabeledByRef !== undefined)
    {
      newLabeledByTarget = findNodeHandle(this.state.labeledByTargetRef.current as
             | null
             | number
             | React.Component<any, any, any>
             | React.ComponentClass<any, any>);
    }

    const currentDescribedByRef = this.state.describedByTargetRef?.current;
    if (currentDescribedByRef !== null && currentDescribedByRef !== undefined)
    {
      newDescribedByTarget = findNodeHandle(this.state.describedByTargetRef.current as
        | null
        | number
        | React.Component<any, any, any>
        | React.ComponentClass<any, any>);
    }

    if (newDescribedByTarget !== this.state.describedByTarget || newLabeledByTarget !== this.state.labeledByTarget)
    {
      this.setState({
        describedByTarget: newDescribedByTarget,
        labeledByTarget: newLabeledByTarget,
        describedByTargetRef: this.state.describedByTargetRef,
        labeledByTargetRef: this.state.labeledByTargetRef});
    }
  }

  public componentDidMount() {
    this.ensureRefStateIsResolved();
  }

  public componentDidUpdate() {
    this.ensureRefStateIsResolved();
  }

  public static getDerivedStateFromProps(
    nextProps: IViewWin32Props,
    prevState: ViewWin32State,
  ): ViewWin32State {
    let nextState = prevState;

    nextState.labeledByTargetRef = nextProps?.accessibilityLabeledBy;
    nextState.describedByTargetRef = nextProps?.accessibilityDescribedBy;

    return nextState;
  }

  public render() {
    if (__DEV__) {
      React.Children.toArray(this.props.children).forEach(item => {
        if (typeof item === 'string') {
          console.error(`Unexpected text node: ${item}. A text node cannot be a child of a <View>.`);
        }
      });
    }

    if (this.props.accessibilityDescribedBy?.current !== null && this.props.accessibilityDescribedBy?.current !== undefined)
    {
      console.error('somehow we got current without processing it ever... do I just move it into render?');
    }

    if (this.props.accessibilityLabeledBy?.current !== null && this.props.accessibilityLabeledBy?.current !== undefined)
    {
      console.error('somehow we got current without processing it ever... do I just move it into render?');
    }

    // Remove props that have been derived and stored in state
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const {accessibilityLabeledBy, accessibilityDescribedBy, ...rest} = this.props;

    return <RN.View
      {...(rest as InnerViewProps)}
      {...((this.state.labeledByTarget !== null) ? {accessibilityLabeledBy:this.state.labeledByTarget} : {})}
      {...((this.state.describedByTarget !== null) ? {accessibilityDescribedBy:this.state.describedByTarget} : {})} />;
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
