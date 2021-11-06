/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {View} from 'react-native';
import {TextBlock, TextBox, Grid, TextAlignment, Button, SymbolIcon, Symbol} from 'react-native-xaml';

class XAMLExamples extends React.Component {
  public render() {
    return (
      <View>
        <TextBlock text='simple TextBlock'/>
        <TextBlock text='right aligned TextBlock' textAlignment={TextAlignment.Right} />
        <TextBox text="some text in a TextBox" />
        <Grid gridLayout={{ rows: [40, 40], columns: [200, 200, 200] }}>
          <Button content="Button 1" gridRow={1} gridColumn={2}>
            <SymbolIcon symbol={Symbol.Flag}/>
          </Button>
        </Grid>
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'XAML';
export const category = 'UI';
export const description = 'Usage of React Native XAML controls';
export const examples = [
  {
    title: 'XAML examples',
    render: function(): JSX.Element {
      return <XAMLExamples />;
    },
  },
];
