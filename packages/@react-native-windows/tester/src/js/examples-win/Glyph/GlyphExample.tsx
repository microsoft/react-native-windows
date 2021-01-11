/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {Text, View} from 'react-native';
import {Glyph} from 'react-native-windows';

class GlyphExamples extends React.Component {
  public render() {
    return (
      <View>
        <Text>Glyph: default Accent color</Text>
        <Glyph
          style={{width: 32, height: 32}}
          emSize={24}
          fontUri="ms-appx:///Assets/seguiemj.ttf"
          glyph="300"
        />
        <Text>Glyph: red</Text>
        <Glyph
          style={{width: 32, height: 32, color: 'red'}}
          emSize={24}
          fontUri="ms-appx:///Assets/seguiemj.ttf"
          glyph="300"
        />
        <Text>Multiple glyphs: red</Text>
        <Glyph
          style={{width: 150, height: 32, color: 'red'}}
          emSize={24}
          fontUri="ms-appx:///Assets/seguiemj.ttf"
          glyph="300;304;703;802;803"
        />
        <Text>Multiple glyphs</Text>
        <Glyph
          style={{width: 150, height: 32, color: 'red'}}
          emSize={24}
          colorEnabled={true}
          fontUri="ms-appx:///Assets/seguiemj.ttf"
          glyph="804;805;806;807;808;809"
        />
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'Glyph UWP';
export const category = 'UI';
export const description = 'Usage of Glyph control.';
export const examples = [
  {
    title: 'Glyph examples',
    render: function(): JSX.Element {
      return <GlyphExamples />;
    },
  },
];
