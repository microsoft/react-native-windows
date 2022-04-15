/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, View, TouchableHighlight, Text, Button, TextInput} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View>
      <TouchableHighlight onPress={()=>{}} style={{height: 200, width: 200, backgroundColor: 'blue'}} accessible={false}>
        <Text>Hello!</Text>
      </TouchableHighlight>
      <View>
        <Text>Hello 2</Text>
        <Button onPress={()=> {}} title="button"/>
        <TextInput focusable={false}/>
      </View>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
