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
const {
  StyleSheet,
  Text,
  TouchableHighlight,
  View,
} = ReactNative;

class RNTesterHeaderWindows extends React.Component {
  constuctor(props: {
    title: ?string,
    onPress: Function,
    style: ?any,
  }) {
      
  }
  render(): ?ReactElement {
    return (
      <View style={[styles.header, this.props.style]}>
        <TouchableHighlight
          onPress={this.props.onPress}
          style={[styles.button]}>
          <Text
            style={[styles.menu]}>
            Menu
          </Text>
        </TouchableHighlight>
        <View style={[styles.titleContainer]}>
          <Text style={[styles.title]}>{this.props.title}</Text>
        </View>
      </View>
    );
  }
}

var styles = StyleSheet.create({
  header: {
    flexDirection: 'row',
  },
  button: {
    backgroundColor: 'blue',
    height: 36,
    padding: 8,
  },
  menu: {
    color: 'white',
    flex: 1,
    fontSize: 15,
    fontWeight: '500',
    textAlign: 'center',
    textAlignVertical: 'center',
  },
  titleContainer: {
    alignItems: 'center',
    flex: 1,
    padding: 6,
  },
  title: {
    fontSize: 17,      
    fontWeight: '500',
  },
});

module.exports = RNTesterHeaderWindows;
