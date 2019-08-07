/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {StyleSheet, requireNativeComponent} from 'react-native';
import {ISwitchProps, ISwitchChangeEvent} from './SwitchProps';

const styles = StyleSheet.create({
  rctSwitch: {height: 31, width: 51},
});

const RCTSwitch = requireNativeComponent('RCTSwitch');

/**
 * A visual toggle between two mutually exclusive states.
 *
 * This is a controlled component that requires an `onValueChange` callback that
 * updates the `value` prop in order for the component to reflect user actions.
 * If the `value` prop is not updated, the component will continue to render the
 * supplied `value` prop instead of the expected result of any user actions.
 *
 * @keyword switch
 * @keyword toggle
 */
class Switch extends React.Component<ISwitchProps> {
  // tslint:disable-next-line:no-any
  private _rctSwitch: any;

  public constructor(props: ISwitchProps) {
    super(props);
    this._rctSwitch = React.createRef();
  }

  public render(): JSX.Element {
    const props = {...this.props};
    props.onResponderTerminationRequest = () => false;
    props.onStartShouldSetResponder = () => true;
    props.style = [styles.rctSwitch, this.props.style];
    props.value = this.props.value === true;
    props.accessibilityRole = this.props.accessibilityRole
      ? this.props.accessibilityRole
      : 'button';

    return (
      <RCTSwitch {...props} onChange={this._onChange} ref={this._setRef} />
    );
  }

  private _setRef = (switchRef: Switch /*RCTSwitch*/) => {
    this._rctSwitch = switchRef;
  };

  private _onChange = (event: ISwitchChangeEvent) => {
    if (this._rctSwitch) {
      // Force value of native switch in order to control it.
      const value = this.props.value === true;
      this._rctSwitch.setNativeProps({value});
    }

    this.props.onChange && this.props.onChange(event);
    this.props.onValueChange &&
      this.props.onValueChange(event.nativeEvent.value);
  };
}

module.exports = Switch;
