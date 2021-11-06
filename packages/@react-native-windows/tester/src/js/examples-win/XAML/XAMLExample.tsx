/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react'
import { View } from 'react-native';
import { TextBlock, TextBox, Grid, TextAlignment, Button, SymbolIcon, Symbol, StackPanel, Orientation, VerticalAlignment, HorizontalAlignment, Path, WinUI, WinUIEnums, FontStyle, FontWeights, Image, Border } from 'react-native-xaml';

class XAMLExamples extends React.Component {
  public render() {
    return (
      <View testID='ReactNativeXAMLRoot'>
        <TextBlock text='simple TextBlock' />
        <TextBlock text='right aligned TextBlock'
          textAlignment={TextAlignment.Right}
          foreground={'red'}
          fontStyle={FontStyle.Italic}
          fontWeight={FontWeights.SemiLight}
        />
        <TextBox text="some text in a TextBox" />
        <Grid gridLayout={{ rows: [40, 40], columns: [200, 200, 200] }} background="green">
          <Border gridRow={1} gridColumn={2} background="blue" xamlMargin={4}>
            <Button horizontalAlignment={HorizontalAlignment.Center}>
              <StackPanel orientation={Orientation.Horizontal}>
                <SymbolIcon symbol={Symbol.Flag} foreground="red" />
                <TextBlock text="Button 1" foreground="white" />
              </StackPanel>
            </Button>
          </Border>
        </Grid>
        <Path
          data="M14,2H12V1H11V2H5V1H4V2H2V14H14ZM4,3V4H5V3h6V4h1V3h1V5H3V3Zm9,10H3V6H13ZM7,10V7H4v3ZM5,8H6V9H5Z"
          width={16}
          height={16}
          fill="red"
          horizontalAlignment={HorizontalAlignment.Center}
          verticalAlignment={VerticalAlignment.Center}
        />
        <Image
          source="data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4gPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyOCAyOCIgZmlsbD0ibm9uZSI+PHBhdGggZD0iTTEzLjEyNSAwSDBWMTMuMTI1SDEzLjEyNVYwWiIgZmlsbD0iI0YyNTAyMiI+PC9wYXRoPjxwYXRoIGQ9Ik0yOCAwSDE0Ljg3NVYxMy4xMjVIMjhWMFoiIGZpbGw9IiM3RkJBMDAiPjwvcGF0aD48cGF0aCBkPSJNMTMuMTI1IDE0Ljg3NUgwVjI4SDEzLjEyNVYxNC44NzVaIiBmaWxsPSIjMDBBNEVGIj48L3BhdGg+PHBhdGggZD0iTTI4IDE0Ljg3NUgxNC44NzVWMjhIMjhWMTQuODc1WiIgZmlsbD0iI0ZGQjkwMCI+PC9wYXRoPjwvc3ZnPiA="
          width={80}
          height={80} />

        <WinUI.ProgressRing isActive={true} width={40} />
        <WinUI.InfoBar
          message="here is the message"
          title="title"
          isOpen={true}
          severity={WinUIEnums.InfoBarSeverity.Success}
          horizontalAlignment={HorizontalAlignment.Center}
        />

      </View>
    );
  }
}

export const displayName = (_undefined?: string) => { };
export const title = 'XAML';
export const category = 'UI';
export const description = 'Usage of React Native XAML controls';
export const examples = [
  {
    title: 'XAML examples',
    render: function (): JSX.Element {
      return <XAMLExamples />;
    },
  },
];
