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
      <TouchableHighlight onPress={()=>{}} style={{height: 200, width: 200, backgroundColor: 'blue'}}>
        <Text>Hello!</Text>
      </TouchableHighlight>
      <View accessible={false}>
        <Text>Hello 2</Text>
        <Button onPress={()=> {}} title="button"/>
        <TouchableHighlight onPress={()=>{}} >
          <Text>Here!</Text>
        </TouchableHighlight>
        <TextInput/>
      </View>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
