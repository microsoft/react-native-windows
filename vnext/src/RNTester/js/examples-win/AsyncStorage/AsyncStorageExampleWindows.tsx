/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {
  AsyncStorage,
  Button,
  StyleSheet,
  FlatList,
  TextInput,
  Text,
  View,
} from 'react-native';

interface AsyncStorageExampleState {
  asyncStorageData: [string, string][];
  newName: string;
  newValue: string;
}

class AsyncStorageExample extends React.Component<{}, AsyncStorageExampleState> {
  state = {
    asyncStorageData: [],
    newName: "",
    newValue: ""
  }

  componentDidMount() {
    AsyncStorage.getAllKeys().then(allKeys => {
      AsyncStorage.multiGet(allKeys).then(values => {
        this.setState({ asyncStorageData: values });
      });
    });
  }

  updateAsyncStorageData(key: string, value: string) {
    this.setState((prevState, props) => {
      let asyncStorageData = [...prevState.asyncStorageData];
      for (var kvp of asyncStorageData) {
        if (kvp[0] == key) {
          kvp[1] = value;
        }
      }
      return { asyncStorageData: asyncStorageData };
    });
  }

  makeOnUpdateKey(key: string) {
    return (value: string) => {
      AsyncStorage.setItem(key, value);
      this.updateAsyncStorageData(key, value);
    };
  }

  makeOnRemoveEntryPress(key: string) {
    return () => {
      AsyncStorage.removeItem(key);
      this.setState((prevState, props) => {
        let asyncStorageData = prevState.asyncStorageData.filter(kvp => kvp[0] != key);
        return { asyncStorageData: asyncStorageData };
      });
    };
  }

  setNewName = (name: string) => {
    this.setState({ newName: name });
  }

  setNewValue = (value: string) => {
    this.setState({ newValue: value });
  }

  onAddEntryPress = () => {
    AsyncStorage.setItem(this.state.newName, this.state.newValue);
    this.updateAsyncStorageData(this.state.newName, this.state.newValue);
  }

  onClearAllKeysPress = () => {
    AsyncStorage.clear();
    this.setState({ asyncStorageData: [] });
  }

  render() {
    return (
      <View style={styles.container}>
        <Button title="Clear All Keys" onPress={this.onClearAllKeysPress} />
        <FlatList
          data={this.state.asyncStorageData}
          renderItem={({ item }) => (
            <View style={styles.keyValue} >
              <Text>{item[0]}:</Text>
              <TextInput
                autoCapitalize="none"
                onChangeText={this.makeOnUpdateKey(item[0])}
                style={styles.textInput}
                value={item[1] as string}
              />
              <Button title="Remove" onPress={this.makeOnRemoveEntryPress(item[0])} />
            </View>
          )}
        />
        <Text>Add a new entry:</Text>
        <View style={styles.keyValue}>
          <Text>Name:</Text>
          <TextInput
            autoCapitalize="none"
            onChangeText={this.setNewName}
            placeholder="<New Key>"
            style={styles.textInput} />
        </View>
        <View style={styles.keyValue}>
          <Text>Value:</Text>
          <TextInput
            autoCapitalize="none"
            onChangeText={this.setNewValue}
            placeholder="<New Value>"
            style={styles.textInput} />
        </View>
        <Button title="Add Entry" onPress={this.onAddEntryPress} />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  button: {
    backgroundColor: "#CCCCCC",
    padding: 10
  },
  container: {
    flex: 1,
    alignItems: 'center',
    marginTop: 80,
  },
  keyValue: {
    alignItems: 'baseline',
    flexDirection: 'row',
  },
  textInput: {
    margin: 5,
    height: 36,
    borderWidth: 1,
    width: 100,
  },
});

export const displayName = (_undefined?: string) => { };
export const title = 'AsyncStorage Windows';
export const description = 'Usage of AsyncStorage';
export const examples = [
  {
    title: 'AsyncStorage',
    render: function (): JSX.Element {
      return <AsyncStorageExample />;
    },
  },
];
