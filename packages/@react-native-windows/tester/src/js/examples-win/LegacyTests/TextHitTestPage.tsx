/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {
  Button,
  GestureResponderEvent,
  StyleSheet,
  Text,
  TouchableWithoutFeedback,
  View,
} from 'react-native';

interface IPressableTextState {
  count: number;
  addView: boolean;
  isPressable: boolean;
}

const styles = StyleSheet.create({
  container: {
    alignItems: 'flex-start',
  },
});

export class PressableTextTests extends React.Component<
  {},
  IPressableTextState
> {
  constructor(props: any) {
    super(props);
    this.state = {
      count: 0,
      addView: false,
      isPressable: true,
    };
  }
  public render() {
    const pressableStyle = {backgroundColor: 'yellow'};
    const alternateStyle = {backgroundColor: 'pink'};
    const increment = () => this.setState({count: this.state.count + 1});
    const toggledProps = this.state.isPressable
      ? {
          onPress: (e: GestureResponderEvent) => {
            increment();
            e.stopPropagation();
          },
          style: alternateStyle,
        }
      : {};
    return (
      <View style={styles.container}>
        <Text testID="pressed-state">Pressed: {this.state.count} times.</Text>
        <Text testID="virtual-text">
          Nested{' '}
          <Text>
            pressable{' '}
            <Text>
              text:{' '}
              <Text onPress={increment} style={pressableStyle}>
                Click here
              </Text>
            </Text>
          </Text>
        </Text>
        <Text testID="nested-text">
          Nested text inside pressable text:{' '}
          <Text onPress={increment} style={pressableStyle}>
            <Text>Click here</Text>
          </Text>
        </Text>
        <Text testID="multiline-text">
          Multiline pressable test:{' '}
          <Text onPress={increment} style={pressableStyle}>
            {'Click here\nor click here.'}
          </Text>
        </Text>
        <Text testID="multiline-rtl-text">
          Multiline pressable RTL text:{' '}
          <Text onPress={increment} style={pressableStyle}>
            {'أحب اللغة\nالعربية'}
          </Text>
        </Text>
        <Text testID="rtl-text">
          RTL text in LTR flow direction:{' '}
          <Text onPress={increment} style={pressableStyle}>
            أحب اللغة العربية
          </Text>
        </Text>
        <Text style={{direction: 'rtl'}} testID="rtl-rtl-flow-text">
          RTL text in RTL flow direction:{' '}
          <Text onPress={increment} style={pressableStyle}>
            أحب اللغة العربية
          </Text>
        </Text>
        <Text style={{direction: 'rtl'}} testID="ltr-rtl-flow-text">
          LTR text in RTL flow direction:{' '}
          <Text onPress={increment} style={pressableStyle}>
            Click here
          </Text>
        </Text>
        <Text testID="bidirectional-text">
          Bidirectional text in a single run:{' '}
          <Text onPress={increment} style={pressableStyle}>
            أحب اللغة العربية hello
          </Text>
        </Text>
        <Text testID="separate-bidirectional-text">
          Bidirectional text in separate runs:{' '}
          <Text onPress={increment} style={pressableStyle}>
            {'أحب اللغة العربية'}
            {' hello'}
          </Text>
        </Text>
        <Text testID="inserted-text">
          Add pressable inline text child:{' '}
          <Text
            onPress={() => this.setState({addView: !this.state.addView})}
            style={pressableStyle}>
            Click to add{' '}
          </Text>
          {this.state.addView ? (
            <Text
              onPress={() => {
                this.setState({addView: false});
                increment();
              }}
              style={alternateStyle}>
              Click to <Text>remove</Text>
            </Text>
          ) : null}
        </Text>
        <TouchableWithoutFeedback
          onPress={() => this.setState({isPressable: !this.state.isPressable})}>
          <Text testID="toggled-text">
            Click anywhere to toggle pressability:{' '}
            <Text {...toggledProps}>Click here</Text>
          </Text>
        </TouchableWithoutFeedback>
        <Text>Wrapped text pressability:</Text>
        <View style={{flexDirection: 'row'}}>
          <Text
            style={{maxWidth: 35, direction: 'rtl'}}
            testID="wrapped-ltr-rtl-flow-text">
            <Text onPress={increment} style={pressableStyle}>
              abcdef
            </Text>
          </Text>
        </View>
        <Button
          testID="clear-state-button"
          title="Clear State"
          onPress={() =>
            this.setState({count: 0, addView: false, isPressable: false})
          }
        />
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'LegacyTextHitTestTest';
export const description = 'Legacy e2e test for Text hit testing';
export const examples = [
  {
    render: function () {
      return <PressableTextTests />;
    },
  },
];
