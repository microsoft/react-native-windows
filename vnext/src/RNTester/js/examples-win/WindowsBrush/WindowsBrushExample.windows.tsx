/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Text, View, Button} from 'react-native';

class WindowsBrushExample extends React.Component {
  _onPress = () => {};

  public render() {
    return (
      <View>
        <Text
          // @ts-ignore
          style={{color: {windowsbrush: 'SystemAccentColorLight3'}}}>
          Sample Text
        </Text>
        <Button
          title="Sample Button"
          // @ts-ignore
          color={{windowsbrush: 'SystemAccentColorLight3'}}
          onPress={this._onPress}
        />
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'WindowsBrush';
export const description = 'Usage of windowsbrush for color props';
export const examples = [
  {
    title: 'WindowsBrush control',
    render: function(): JSX.Element {
      return <WindowsBrushExample />;
    },
  },
];
