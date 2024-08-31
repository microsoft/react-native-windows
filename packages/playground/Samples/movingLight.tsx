/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React, { useState } from 'react';
import {AppRegistry, Text, View} from 'react-native';
import {MovingLight} from 'sample-custom-component';

const colors = ['white', 'red', 'green', 'blue'];

const MovingLightExample = () => {
  const [colorIndex, setColorIndex] = useState(0);
  const [eventValue, setEventValue] = useState('No Event yet');

  const color = colors[colorIndex];
  const size = 25 + colorIndex * 25
  return (
    <View>
      <Text>{`Size: ${size}`}</Text>
      <Text>{`Color: ${color}`}</Text>
      <Text>{`Last Event Value: ${eventValue}`}</Text>
      <MovingLight style={{width: 500, height: 500, backgroundColor:'white'}} size={size} color={color} eventParam={color} onSomething={(e) => {
        setColorIndex((colorIndex + 1) % colors.length);
        setEventValue(e.nativeEvent.value);
      }} />
    </View>
  );
}

export default class Bootstrap extends React.Component {
  render() {
    return (
      <MovingLightExample />
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);