/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React, {useState} from 'react';
import {AppRegistry, Pressable, Text, View} from 'react-native';

export const Bootstrap: React.FunctionComponent<{}> = () => {
  const [timesPressed, setTimesPressed] = useState(0);

  let textLog = '';
  if (timesPressed > 1) {
    textLog = timesPressed + 'x onPress';
  } else if (timesPressed > 0) {
    textLog = 'onPress';
  }

  return (
    <>
      <View>
        <Pressable
          accessibilityRole="button"
          onPressOut={() => {
            setTimesPressed(current => current + 1);
          }}>
          <Text>Press Me</Text>
        </Pressable>
      </View>
      <View>
        <Text>{textLog}</Text>
      </View>
    </>
  );
};

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
