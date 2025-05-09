/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, Button, Modal, View} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View
        accessible={true}>
        <Button
          title="Open Modal"
          onPress={() => {
            setModal1(!modal1);
          }} />
        <Modal visible={modal1}>
          <View style={{width: 500, height: 50}}>
            <Text>This is a simple Modal</Text>
            <Button
              title="Close Modal"
              onPress={() => {
                setModal1(!modal1);
              }} />
          </View>
        </Modal>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
