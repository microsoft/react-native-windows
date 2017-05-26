/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @flow
 */
'use strict';

const React = require('react');
const ReactNative = require('react-native');
const StyleSheet = require('StyleSheet');
const RNTesterBlock = require('RNTesterBlock');
const RNTesterPage = require('RNTesterPage');

const {
  Picker,
  Text,
  TouchableWithoutFeedback,
} = ReactNative;
const Item = Picker.Item;

const PickerExample = React.createClass({

  statics: {
    title: '<Picker>',
    description: 'Provides multiple options to choose from, using a dropdown menu.',
  },

  getInitialState: function() {
    return {
      selected1: 'key1',
      selected2: 'key1',
      selected3: 'key1',
      color: 'red',
    };
  },

  render: function() {
    return (
      <RNTesterPage title="<Picker>">
        <RNTesterBlock title="Basic Picker">
          <Picker
            style={styles.picker}
            selectedValue={this.state.selected1}
            onValueChange={this.onValueChange.bind(this, 'selected1')}>
            <Item label="hello" value="key0" />
            <Item label="world" value="key1" />
          </Picker>
        </RNTesterBlock>
        <RNTesterBlock title="Disabled picker">
          <Picker style={styles.picker} enabled={false} selectedValue={this.state.selected1}>
            <Item label="hello" value="key0" />
            <Item label="world" value="key1" />
          </Picker>
        </RNTesterBlock>
        <RNTesterBlock title="Picker with no listener">
          <Picker style={styles.picker}>
            <Item label="hello" value="key0" />
            <Item label="world" value="key1" />
          </Picker>
          <Text>
            Cannot change the value of this picker because it doesn't update selectedValue.
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Colorful pickers">
          <Picker
            style={[styles.picker, {color: 'white', backgroundColor: '#333'}]}
            selectedValue={this.state.color}
            onValueChange={this.onValueChange.bind(this, 'color')}>
            <Item label="red" color="red" value="red" />
            <Item label="green" color="green" value="green" />
            <Item label="blue" color="blue" value="blue" />
          </Picker>
        </RNTesterBlock>
      </RNTesterPage>
    );
  },

  onValueChange: function(key: string, value: string) {
    const newState = {};
    newState[key] = value;
    this.setState(newState);
  },
});

var styles = StyleSheet.create({
  picker: {
    width: 100,
  },
});

module.exports = PickerExample;
