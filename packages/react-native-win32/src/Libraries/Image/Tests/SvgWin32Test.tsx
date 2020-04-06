import * as React from 'react';
import { View, StyleSheet, Text } from 'react-native';
import { Svg, Path, Circle, Defs, RadialGradient, Stop, Line, SvgCssUri, G, Rect, Use, Polygon } from 'react-native-svg';

const styles = StyleSheet.create({
  svg: {
    backgroundColor: 'green',
    color:'purple',
  },
});

import TestSvg from './img/accessible-icon-brands.svg';

export const title = '<SvgWin32Test>';
export const displayName = 'Svg Win32 test';
export const description = 'Svg Win32 test';
export const examples = [
    {
      title: 'Win32 Svg control test',
      description: 'Test ImageSvg',
      render(): JSX.Element {
        return (
          <View>
            <Text>Rect</Text>
            <Svg width="50" height="50">
              <Rect x="10" y="10" width="20" height="20" fill="red" stroke="black"/>
            </Svg>

            <Text>Circle</Text>
            <Svg width="50" height="50">
              <Circle cx="25" cy="25" r="20" fill="red" stroke="black"/>
            </Svg>

            <Text>Line</Text>
            <Svg width="50" height="50">
              <Line x1="10" y1="10" x2="20" y2="20" fill="red" stroke="black"/>
            </Svg>

            <Text>Path</Text>
            <Svg width="100" height="100">
              <Path d="M 0 0 A 10 10 0 0,1 0 100" fill="magenta" stroke="purple" id="path"/>
            </Svg>

            <Text>Polygon</Text>
            <Svg height="100" width="100">
              <Polygon
                points="40,5 70,80 25,95"
                fill="lime"
                stroke="purple"
                strokeWidth="1"
              />
            </Svg>

            <Text>RadialGradient</Text>
            <Svg height="100" width="100" style={{ backgroundColor: 'black' }} viewBox="0 0 10 10" color="yellow">
              <Defs>
                <RadialGradient
                  id="grad"
                  cx="10"
                  cy="10"
                  r="8"
                  fx="10"
                  fy="10"
                  gradientUnits="objectBoundingBox"
                >
                  <Stop offset="0%" stopColor="yellow" stopOpacity=".5" />
                  <Stop offset="50%" stopColor="red" stopOpacity="1" />
                  <Stop offset="100%" stopColor="blue" stopOpacity="1" />
                </RadialGradient>
              </Defs>
              <Line x1="0" y1="0" x2="60" y2="60" fill="red" stroke="blue" strokeWidth="5"/>
            </Svg>

            <Text>Rect and Circle via Defs and Use</Text>
            <Svg height="200" width="200">
              <Defs>
                <G id="shape">
                  <G>
                    <Rect x="120" y="120" width="20" height="50" fill="red" />
                    <Circle cx="100" cy="100" r="20" fill="blue" stroke="black"/>
                  </G>
                </G>
              </Defs>
              <Use href="#shape" x="20" y="0" />
              <Use href="#shape" x="170" y="0" />
            </Svg>
            <Text>Bundled svg </Text>
            <TestSvg width={200} height={200} color="red"/>
            <Text>Remotely retrieved svgs</Text>
            <SvgCssUri style={styles.svg} viewBox="0 0 200 200" width="100" height="100" uri="https://upload.wikimedia.org/wikipedia/commons/8/84/Example.svg" />
            <SvgCssUri x="50" y="50" viewBox="0 0 500 500" style={styles.svg} width="100" height="100" uri="http://10.122.222.112:8080/accessible-icon-brands.svg" />

          </View>
        );
      },
    },
  ];
