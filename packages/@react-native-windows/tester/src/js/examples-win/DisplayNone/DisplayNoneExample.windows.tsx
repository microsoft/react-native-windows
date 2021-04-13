/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {Switch, Text, TextInput, View} from 'react-native';
const RNTesterBlock = require('../../components/RNTesterBlock');
const RNTesterPage = require('../../components/RNTesterPage');

export class DisplayNoneExample extends React.Component<{}> {
  state = {
    displayNone: false,
    displayNone2: false,
    textState: 'TextInput1...',
    textState2: 'TextInput2...',
    textState3: 'TextInput3...',
    textState4: 'TextInput3...',
  };

  public render() {
    return (
      <RNTesterPage>
        <RNTesterBlock title="Display:none style with TextInput">
          <View>
            <View testID="view-component-test-base">
              <View style={{width: 1074}}>
                <Text>Useless Switch.</Text>
                <Switch />
                <Text>Useless Switch2.</Text>
                <Switch />
              </View>
              <View>
                <TextInput
                  style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                  onChangeText={text => this._handleChangeText(text)}
                  value={this.state.textState}
                />
              </View>
              <View>
                <TextInput
                  style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                  onChangeText={text => this._handleChangeText2(text)}
                  value={this.state.textState2}
                />
              </View>
            </View>
          </View>
          <View>
            <View style={{width: 1074}} testID="view-component-switch-view">
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
                value={this.state.textState3}
              />
            </View>
            <View style={{display: this.state.displayNone2 ? 'none' : 'flex'}}>
              <TextInput
                style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                onChangeText={text => this._handleChangeText2(text)}
                value={this.state.textState4}
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
