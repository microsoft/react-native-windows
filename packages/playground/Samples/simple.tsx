/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React, {useState} from 'react';
import {AppRegistry, View, Text} from 'react-native';

const Bootstrap = () => {
  const [lastEvent, setLastEvent] = useState('Click the box');

  return (
    <View style={{padding: 20}}>
      <View
        accessible={true}
        accessibilityValue={{now: 10, min: 0, max: 20}}
        style={{
          borderRadius: 30,
          width: 200,
          height: 200,
          margin: 10,
          backgroundColor: 'magenta',
          justifyContent: 'center',
          alignItems: 'center',
        }}
        onPointerDown={e => {
          const {button, buttons} = e.nativeEvent;
          const label =
            button === 0
              ? 'Left'
              : button === 1
                ? 'Middle'
                : button === 2
                  ? 'Right'
                  : `Unknown(${button})`;
          setLastEvent(
            `${label} button pressed (button=${button}, buttons=${buttons})`,
          );
        }}
        onPointerUp={e => {
          const {button, buttons} = e.nativeEvent;
          const label =
            button === 0
              ? 'Left'
              : button === 1
                ? 'Middle'
                : button === 2
                  ? 'Right'
                  : `Unknown(${button})`;
          setLastEvent(
            `${label} button released (button=${button}, buttons=${buttons})`,
          );
        }}
        // @ts-ignore
        onClick={() => setLastEvent('onClick fired')}>
        <Text style={{color: 'white', fontWeight: 'bold'}}>Click Me</Text>
      </View>
      <Text style={{marginTop: 10, fontSize: 16}}>{lastEvent}</Text>
    </View>
  );
};

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
