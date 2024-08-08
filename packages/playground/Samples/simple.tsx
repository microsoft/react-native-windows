/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {
  AppRegistry,
  View,
  Button,
  SafeAreaView,
  Modal,
  Text,
} from 'react-native';
const Bootstrap = () => {
  const [modalVisible, setModalVisible] = React.useState(false);
  return (
    <SafeAreaView>
      <Button
        title="Open Modal"
        onPress={() => setModalVisible(!modalVisible)}
      />
      <Text style={{textAlign:'center'}}>Modal visibility: {String(modalVisible)}</Text>
      <Modal visible={modalVisible} style={{backgroundColor: 'yellow'}}>
        <SafeAreaView style={{backgroundColor: 'yellow', width: 200, height: 200}}>
          <Text style={{backgroundColor: 'green', width: 100, height: 100}}>I should be in the Modal!</Text>
          <Text style={{backgroundColor: 'green', width: 100, height: 100}}>I should be in the Modal 2!</Text>
          <Button title='Close Modal' onPress={() => setModalVisible(!modalVisible)}/>
        </SafeAreaView>
      </Modal>
      <View
        accessible={true}
        style={{borderRadius: 30, width: 60, height: 60, margin: 10}}>
        <View style={{backgroundColor: 'magenta', width: 60, height: 60}} />
      </View>
    </SafeAreaView>
  );
};
AppRegistry.registerComponent('Bootstrap', () => Bootstrap);