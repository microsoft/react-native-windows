/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {Switch, Text, TextInput, View} from 'react-native';
const RNTesterBlock = require('../../components/RNTesterBlock');
const RNTesterPage = require('../../components/RNTesterPage');

export class DisplayNoneExample extends React.Component<{}> {
  state = {
    displayNone: false,
    displayNone2: false,
    textState: 'TextInput1...',
    textState2: 'TextInput2...',
  };

  public render() {
    return (
      <RNTesterPage>
        <RNTesterBlock title="Display:none style with TextInput">
          <View testID="view-component-switch-view">
            <View>
              <Text>toggle display:none TextInput1</Text>
              <Switch
                onValueChange={this._handlePress}
                value={this.state.displayNone}
              />
              <Text>toggle display:none TextInput2</Text>
              <Switch
                onValueChange={this._handlePress2}
                value={this.state.displayNone2}
                testID="toggle-display:none"
              />
            </View>
            <View style={{display: this.state.displayNone ? 'none' : 'flex'}}>
              <TextInput
                style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                onChangeText={text => this._handleChangeText(text)}
                value={this.state.textState}
              />
            </View>
            <View style={{display: this.state.displayNone2 ? 'none' : 'flex'}}>
              <TextInput
                style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                onChangeText={text => this._handleChangeText2(text)}
                value={this.state.textState2}
              />
            </View>
          </View>
        </RNTesterBlock>
      </RNTesterPage>
    );
  }

  _handlePress = () => {
    this.setState({displayNone: !this.state.displayNone});
  };

  _handlePress2 = () => {
    this.setState({displayNone2: !this.state.displayNone2});
  };

  _handleChangeText = (text: string) => {
    this.setState({textState: text});
  };

  _handleChangeText2 = (text: string) => {
    this.setState({textState2: text});
  };
}

export const displayName = (_undefined?: string) => {};
export const title = 'Display:none Style';
export const description =
  'Style prop which will collapse the element in XAML tree.';
export const examples = [
  {
    render: function(): JSX.Element {
      return <DisplayNoneExample />;
    },
  },
];
