/**
 * Copyright (c) 2015-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 * 
 * Portions copyright for react-native-windows:
 * 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * 
 * @providesModule PickerWindows
 * @flow
 */

'use strict';

var ColorPropType = require('ColorPropType');
var React = require('React');
var ReactPropTypes = require('prop-types');
var StyleSheet = require('StyleSheet');
var StyleSheetPropType = require('StyleSheetPropType');
const ViewPropTypes = require('ViewPropTypes');
var ViewStylePropTypes = require('ViewStylePropTypes');

var processColor = require('processColor');
var requireNativeComponent = require('requireNativeComponent');

var REF_PICKER = 'picker';

var pickerStyleType = StyleSheetPropType({
  ...ViewStylePropTypes,
  color: ColorPropType,
});

type Event = Object;

/**
 * Not exposed as a public API - use <Picker> instead.
 */
class PickerWindows extends React.Component<{
  style?: $FlowFixMe,
  items?: any,
  selected?: number,
  selectedValue?: any,
  enabled?: boolean,
  onValueChange?: Function,
  prompt?: string,
  testID?: string,
}, *> {
  static propTypes = {
    ...ViewPropTypes,
    style: pickerStyleType,
    items: ReactPropTypes.any,
    selected: ReactPropTypes.number,
    selectedValue: ReactPropTypes.any,
    enabled: ReactPropTypes.bool,
    onValueChange: ReactPropTypes.func,
    prompt: ReactPropTypes.string,
    testID: ReactPropTypes.string,
  };

  constructor(props, context) {
    super(props, context);
    var state = this._stateFromProps(props);

    this.state = {
      ...state,
      initialSelectedIndex: state.selectedIndex,
    };
  }

  UNSAFE_componentWillReceiveProps(nextProps) {
    this.setState(this._stateFromProps(nextProps));
  }

  // Translate prop and children into stuff that the native picker understands.
  _stateFromProps = (props) => {
    var selectedIndex = -1;
    const items = React.Children.map(props.children, (child, index) => {
      if (child.props.value === props.selectedValue) {
        selectedIndex = index;
      }
      const childProps = {
        value: child.props.value,
        label: child.props.label,
      };
      if (child.props.color) {
        childProps.color = processColor(child.props.color);
      }
      return childProps;
    });
    return {selectedIndex, items};
  };

  render() {
    var Picker = ComboBoxPicker;

    var nativeProps = {
      enabled: this.props.enabled,
      items: this.state.items,
      onSelect: this._onChange,
      prompt: this.props.prompt,
      selected: this.state.initialSelectedIndex,
      testID: this.props.testID,
      style: [this.props.style],
    };

    return <Picker ref={REF_PICKER} {...nativeProps} />;
  }

  _onChange = (event: Event) => {
    if (this.props.onValueChange) {
      var position = event.nativeEvent.position;
      if (position >= 0) {
        var children = React.Children.toArray(this.props.children);
        var value = children[position].props.value;
        this.props.onValueChange(value, position);
      } else {
        this.props.onValueChange(null, position);
      }
    }
    this._lastNativePosition = event.nativeEvent.position;
    this.forceUpdate();
  };

  componentDidMount() {
    this._lastNativePosition = this.state.initialSelectedIndex;
  }

  componentDidUpdate() {
    // The picker is a controlled component. This means we expect the
    // on*Change handlers to be in charge of updating our
    // `selectedValue` prop. That way they can also
    // disallow/undo/mutate the selection of certain values. In other
    // words, the embedder of this component should be the source of
    // truth, not the native component.
    if (this.refs[REF_PICKER] && this.state.selectedIndex !== this._lastNativePosition) {
      this.refs[REF_PICKER].setNativeProps({selected: this.state.selectedIndex});
      this._lastNativePosition = this.state.selectedIndex;
    }
  }
}

var cfg = {
  nativeOnly: {
    items: true,
    selected: true,
  }
};

var ComboBoxPicker = requireNativeComponent('RCTPicker', PickerWindows, cfg);

module.exports = PickerWindows;
