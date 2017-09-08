/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule PickerWindows
 * @flow
 */

'use strict';

var ColorPropType = require('ColorPropType');
var PropTypes = require('prop-types');
var React = require('React');
var StyleSheet = require('StyleSheet');
var StyleSheetPropType = require('StyleSheetPropType');
var ViewPropTypes = require('ViewPropTypes');

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
class PickerWindows extends React.Component {
  props: {
    style?: $FlowFixMe,
    items?: any,
    selected?: number,
    selectedValue?: any,
    enabled?: boolean,
    onValueChange?: Function,
    prompt?: string,
    testID?: string,
  };

  static propTypes = {
    ...ViewPropTypes,
    style: pickerStyleType,
    items: PropTypes.any,
    selected: PropTypes.number,
    selectedValue: PropTypes.any,
    enabled: PropTypes.bool,
    onValueChange: PropTypes.func,
    prompt: PropTypes.string,
    testID: PropTypes.string,
  };

  componentWillReceiveProps(nextProps: Object) {
    this.setState(this._stateFromProps(nextProps));
  }

  // Translate prop and children into stuff that the native picker understands.
  _stateFromProps = (props) => {
    var selectedIndex = 0;
    let items = React.Children.map(props.children, (child, index) => {
      if (child.props.value === props.selectedValue) {
        selectedIndex = index;
      }
      let childProps = {
        value: child.props.value,
        label: child.props.label,
      };
      if (child.props.color) {
        // $FlowIssue: doesn't like adding properties to objects
        /*::`*/
        childProps.color = processColor(child.props.color);
        /*::`;*/
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
      selected: this.state.selectedIndex,
      testID: this.props.testID,
      style: [this.props.style],
    };

    return <Picker ref={REF_PICKER} {...nativeProps} />;
  }

  _onChange = (event: Object) => {
    if (this.props.onValueChange) {
      var position = event.nativeEvent.position;
      if (position >= 0) {
        var value = this.props.children[position].props.value;
        this.props.onValueChange(value, position);
      } else {
        this.props.onValueChange(null, position);
      }
    }

    // The picker is a controlled component. This means we expect the
    // on*Change handlers to be in charge of updating our
    // `selectedValue` prop. That way they can also
    // disallow/undo/mutate the selection of certain values. In other
    // words, the embedder of this component should be the source of
    // truth, not the native component.
    if (this.refs[REF_PICKER] && this.state.selectedIndex !== event.nativeEvent.position) {
      this.refs[REF_PICKER].setNativeProps({selected: this.state.selectedIndex});
    }
  };

  state = this._stateFromProps(this.props);
}

var cfg = {
  nativeOnly: {
    items: true,
    selected: true,
  }
};

var ComboBoxPicker = requireNativeComponent('RCTPicker', PickerWindows, cfg);

module.exports = PickerWindows;
