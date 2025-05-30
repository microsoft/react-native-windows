/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {Button, StyleSheet, Text, View} from 'react-native';

interface IPressableTextState {
  count: number;
  isSelectable: boolean;
}

const styles = StyleSheet.create({
  container: {
    alignItems: 'flex-start',
  },
  pressable: {
    color: 'blue',
    textDecorationLine: 'underline',
  },
});

export class SelectableTextTests extends React.Component<
  {},
  IPressableTextState
> {
  constructor(props: any) {
    super(props);
    this.state = {
      count: 0,
      isSelectable: false,
    };
  }
  public render() {
    const increment = () => this.setState({count: this.state.count + 1});
    return (
      <View style={styles.container}>
        <Text testID="pressed-state">Pressed: {this.state.count} times.</Text>
        <Text testID="text-example" selectable={this.state.isSelectable}>
          Text before{' '}
          <Text onPress={increment} style={styles.pressable}>
            click here
          </Text>{' '}
          text after
        </Text>
        <Button
          testID="toggle-selectable-button"
          title="Toggle Selectable"
          onPress={() =>
            this.setState({isSelectable: !this.state.isSelectable})
          }
        />
        <Button
          testID="clear-state-button"
          title="Clear State"
          onPress={() => this.setState({count: 0, isSelectable: false})}
        />
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'LegacySelectableTextTest';
export const description = 'Legacy e2e test for selectable Text hit testing';
export const examples = [
  {
    render: function () {
      return <SelectableTextTests />;
    },
  },
];
