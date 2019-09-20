/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {StyleSheet, requireNativeComponent} from 'react-native';
import {ICheckBoxProps, ICheckBoxChangeEvent} from './CheckBoxProps';

const styles = StyleSheet.create({
  rctCheckBox: {
    height: 32,
    width: 32,
  },
});

const RCTCheckBox = requireNativeComponent('RCTCheckBox');

/**
 * Renders a boolean input.
 *
 * This is a controlled component that requires an `onValueChange` callback that
 * updates the `value` prop in order for the component to reflect user actions.
 * If the `value` prop is not updated, the component will continue to render
 * the supplied `value` prop instead of the expected result of any user actions.
 *
 * @keyword checkbox
 * @keyword toggle
 */
class CheckBox extends React.Component<ICheckBoxProps> {
  // tslint:disable-next-line:no-any
  private _rctCheckBox: any;

  public constructor(props: ICheckBoxProps) {
    super(props);
    this._rctCheckBox = React.createRef();
  }

  public render(): JSX.Element {
    const props = {...this.props};
    props.onStartShouldSetResponder = () => true;
    props.onResponderTerminationRequest = () => false;
    props.style = [styles.rctCheckBox, this.props.style];

    return (
      <RCTCheckBox {...props} onChange={this._onChange} ref={this._setRef} />
    );
  }

  private _setRef = (checkBox: CheckBox /*RCTCheckBox*/) => {
    this._rctCheckBox = checkBox;
  };

  private _onChange = (event: ICheckBoxChangeEvent) => {
    if (this._rctCheckBox) {
      this._rctCheckBox.setNativeProps({value: this.props.value});
    }

    // Change the props after the native props are set in case the props
    // change removes the component
    this.props.onChange && this.props.onChange(event);
    this.props.onValueChange &&
      this.props.onValueChange(event.nativeEvent.value);
  };
}

module.exports = CheckBox;
