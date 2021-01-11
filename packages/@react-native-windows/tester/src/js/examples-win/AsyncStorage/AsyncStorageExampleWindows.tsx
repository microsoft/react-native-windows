/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AsyncStorage,
  Button,
  FlatList,
  StyleSheet,
  Text,
  TextInput,
  View,
} from 'react-native';

interface AsyncStorageExampleState {
  asyncStorageData: [string, string][];
  name: string;
  value: string;
}

class AsyncStorageExample extends React.Component<
  {},
  AsyncStorageExampleState
> {
  state = {
    asyncStorageData: [],
    name: '',
    value: '',
  };

  async componentDidMount() {
    const allKeys = await AsyncStorage.getAllKeys();
    const values = await AsyncStorage.multiGet(allKeys);
    this.setState({asyncStorageData: values});
  }

  private updateAsyncStorageData(key: string, value: string) {
    this.setState(prevState => {
      const asyncStorageData = [...prevState.asyncStorageData];
      let foundVal = false;
      for (const kvp of asyncStorageData) {
        if (kvp[0] === key) {
          kvp[1] = value;
          foundVal = true;
          break;
        }
      }
      if (!foundVal) {
        asyncStorageData.push([key, value]);
      }
      return {asyncStorageData: asyncStorageData};
    });
  }

  private makeOnRemoveEntryPress(key: string) {
    return () => {
      void AsyncStorage.removeItem(key);
      this.setState(prevState => {
        const asyncStorageData = prevState.asyncStorageData.filter(
          kvp => kvp[0] !== key,
        );
        return {asyncStorageData: asyncStorageData};
      });
    };
  }

  private readonly setName = (name: string) => {
    this.setState({name: name});
  };

  private readonly setValue = (value: string) => {
    this.setState({value: value});
  };

  private readonly onAddEntryPress = () => {
    void AsyncStorage.setItem(this.state.name, this.state.value);
    this.updateAsyncStorageData(this.state.name, this.state.value);
  };

  private readonly onClearAllKeysPress = () => {
    void AsyncStorage.clear();
    this.setState({asyncStorageData: []});
  };

  render() {
    return (
      <View style={styles.container}>
        <Button title="Clear All Keys" onPress={this.onClearAllKeysPress} />
        <FlatList
          data={this.state.asyncStorageData}
          renderItem={({item}) => (
            <View style={styles.keyValue}>
              <Text>
                "{item[0]}": "{item[1]}"
              </Text>
              <Button
                title="Remove"
                onPress={this.makeOnRemoveEntryPress(item[0])}
              />
            </View>
          )}
        />
        <Text>Add/Update an entry:</Text>
        <View style={styles.keyValue}>
          <Text>Name:</Text>
          <TextInput
            autoCapitalize="none"
            onChangeText={this.setName}
            placeholder="<Key>"
            style={styles.textInput}
          />
        </View>
        <View style={styles.keyValue}>
          <Text>Value:</Text>
          <TextInput
            autoCapitalize="none"
            onChangeText={this.setValue}
            placeholder="<New Value>"
            style={styles.textInput}
          />
        </View>
        <Button title="Add Entry" onPress={this.onAddEntryPress} />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  button: {
    backgroundColor: '#CCCCCC',
    margin: 10,
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
    height: 32,
    borderWidth: 1,
    width: 100,
  },
});

export const displayName = (_undefined?: string) => {};
export const title = 'AsyncStorage Windows';
export const category = 'Basic';
export const description = 'Usage of AsyncStorage';
export const examples = [
  {
    title: 'AsyncStorage',
    render: function(): JSX.Element {
      return <AsyncStorageExample />;
    },
  },
];
