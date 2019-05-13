/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * Portions copyright for react-native-windows:
 * 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * 
 * @format
 * @flow
 */

'use strict';

const React = require('React');
const StyleSheet = require('StyleSheet');

const processColor = require('processColor');
const requireNativeComponent = require('requireNativeComponent');


var ComboBoxPicker = requireNativeComponent('RCTPicker', PickerWindows, {
  nativeOnly: {
    items: true,
    selected: true,
  }
});

const REF_PICKER = 'picker';

import type {SyntheticEvent} from 'CoreEventTypes';
import type {TextStyleProp} from 'StyleSheet';

type PickerWindowsChangeEvent = SyntheticEvent<
  $ReadOnly<{|
    position: number,
  |}>,
>;

type PickerWindowsProps = $ReadOnly<{|
  children?: React.Node,
  style?: ?TextStyleProp,
  selectedValue?: any,
  enabled?: ?boolean,
  onValueChange?: ?(newValue: any, newIndex: number) => mixed,
  prompt?: ?string,
  testID?: string,
|}>;

/**
 * Not exposed as a public API - use <Picker> instead.
 */
class PickerWindows extends React.Component<PickerWindowsProps, *> {
  /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was found
   * when making Flow check .android.js files. */
  constructor(props, context) {
    super(props, context);
    const state = this._stateFromProps(props);

    this.state = {
      ...state,
      initialSelectedIndex: state.selectedIndex,
    };
  }

  /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was found
   * when making Flow check .android.js files. */
  UNSAFE_componentWillReceiveProps(nextProps) {
    this.setState(this._stateFromProps(nextProps));
  }

  // Translate prop and children into stuff that the native picker understands.
  _stateFromProps = props => {
    let selectedIndex = -1;
    const items = React.Children.map(props.children, (child, index) => {
      if (child.props.value === props.selectedValue) {
        selectedIndex = index;
      }
      const childProps = {
        value: child.props.value,
        label: child.props.label,
      };
      if (child.props.color) {
        /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was
         * found when making Flow check .android.js files. */
        childProps.color = processColor(child.props.color);
      }
      return childProps;
    });
    return {selectedIndex, items};
  };

  render() {
    var Picker = ComboBoxPicker;

    const nativeProps = {
      enabled: this.props.enabled,
      items: this.state.items,
      onSelect: this._onChange,
      prompt: this.props.prompt,
      selected: this.state.initialSelectedIndex,
      testID: this.props.testID,
      style: [this.props.style],
      /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was found
       * when making Flow check .android.js files. */
      accessibilityLabel: this.props.accessibilityLabel,
    };

    return <Picker ref={REF_PICKER} {...nativeProps} />;
  }

  _onChange = (event: PickerWindowsChangeEvent) => {
    if (this.props.onValueChange) {
      const position = event.nativeEvent.position;
      if (position >= 0) {
        const children = React.Children.toArray(this.props.children);
        const value = children[position].props.value;
        /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was
         * found when making Flow check .android.js files. */
        this.props.onValueChange(value, position);
      } else {
        this.props.onValueChange(null, position);
      }
    }
    /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was found
     * when making Flow check .android.js files. */
    this._lastNativePosition = event.nativeEvent.position;
    this.forceUpdate();
  };

  componentDidMount() {
    /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was found
     * when making Flow check .android.js files. */
    this._lastNativePosition = this.state.initialSelectedIndex;
  }

  componentDidUpdate() {
    // The picker is a controlled component. This means we expect the
    // on*Change handlers to be in charge of updating our
    // `selectedValue` prop. That way they can also
    // disallow/undo/mutate the selection of certain values. In other
    // words, the embedder of this component should be the source of
    // truth, not the native component.
    if (
      this.refs[REF_PICKER] &&
      /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was found
       * when making Flow check .android.js files. */
      this.state.selectedIndex !== this._lastNativePosition
    ) {
      this.refs[REF_PICKER].setNativeProps({
        selected: this.state.selectedIndex,
      });
      /* $FlowFixMe(>=0.78.0 site=react_native_android_fb) This issue was found
       * when making Flow check .android.js files. */
      this._lastNativePosition = this.state.selectedIndex;
    }
  }
}

module.exports = PickerWindows;
