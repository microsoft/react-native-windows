'use strict';

import React, { useRef, useState } from 'react';
import {Button, Text, View} from 'react-native';
import {MovingLight} from '@rnw-scripts/sample-custom-component';
import type {MovingLightHandle} from '@rnw-scripts/sample-custom-component';

const colors = ['white', 'red', 'green', 'blue'];

const MovingLightExample = () => {
  const [colorIndex, setColorIndex] = useState(0);
  const [eventValue, setEventValue] = useState('No Event yet');
  const myRef = useRef<MovingLightHandle>(null);
  const [lightOn, setLightOn] = useState(true);

  const color = colors[colorIndex];
  const size = 25 + colorIndex * 25
  return (
    <View>
      <Text>{`Size: ${size}`}</Text>
      <Text>{`Color: ${color}`}</Text>
      <Button title='Toggle Light' onPress={()=> { setLightOn(!lightOn); myRef.current!.setLightOn(!lightOn); }}/>
      <Text>{`Last Event Value: ${eventValue}`}</Text>
      <MovingLight ref={myRef} style={{width: 500, height: 500, backgroundColor:'white'}} size={size} color={color} eventParam={color} onSomething={(value) => {
        setColorIndex((colorIndex + 1) % colors.length);
        setEventValue(value);
      }} />
    </View>
  );
}

exports.displayName = 'MovingLightExample';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Moving Light Example';
exports.description =
  'Sample Custom Fabric Native Component';

exports.examples = [
  {
    title: 'Moving Light',
    render: function (): React.ReactNode {
      return (
        <MovingLightExample />
      );
    },
  }
];
