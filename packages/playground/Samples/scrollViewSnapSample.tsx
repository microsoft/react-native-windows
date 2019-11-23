/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  Switch,
  StyleSheet,
  View,
  Text,
  ScrollView,
  TouchableOpacity,
  RefreshControl,
} from 'react-native';

function wait(timeout: number) {
  return new Promise(resolve => {
    setTimeout(resolve, timeout);
  });
}

export default class Bootstrap extends React.Component<{}, any> {
  state = {
    horizontalValue: true,
    snapToStartValue: false,
    snapToEndValue: false,
    zoomValue: false,
    alignToStartValue: true,
    refreshing: false,
    snapToOffsets: true,
    pagingEnabled: false,
  };

  toggleSwitch1 = (value: boolean) => {
    this.setState({horizontalValue: value});
  };

  toggleSwitch2 = (value: boolean) => {
    this.setState({snapToStartValue: value});
  };

  toggleSwitch3 = (value: boolean) => {
    this.setState({snapToEndValue: value});
  };

  toggleSwitch4 = (value: boolean) => {
    this.setState({zoomValue: value});
  };

  toggleSwitch5 = (value: boolean) => {
    this.setState({alignToStartValue: value});
  };

  toggleSwitch6 = (value: boolean) => {
    this.setState({snapToOffsets: value});
  };

  toggleSwitch7 = (value: boolean) => {
    this.setState({pagingEnabled: value});
  };

  onRefresh = () => {
    this.setState({refreshing: true});
    wait(2000).then(() => this.setState({refreshing: false}));
  };

  makeItems = (nItems: number, styles: Object): Array<any> => {
    const items = [];
    for (let i = 0; i < nItems; i++) {
      items[i] = (
        <TouchableOpacity key={i} style={styles}>
          <Text>{'Item ' + i}</Text>
        </TouchableOpacity>
      );
    }
    return items;
  };

  render() {
    const item = (
      <View
        style={{
          flex: 1,
          alignSelf: 'auto',
          flexDirection: 'column',
          justifyContent: 'flex-start',
        }}>
        <View style={{flex: 0.2, alignSelf: 'center', flexDirection: 'row'}}>
          <View
            style={{
              flexDirection: 'column',
              alignSelf: 'stretch',
              justifyContent: 'center',
              padding: 20,
            }}>
            <Text>
              {this.state.horizontalValue ? 'Horizontal ' : 'Vertical '}
            </Text>
            <Switch
              onValueChange={this.toggleSwitch1}
              value={this.state.horizontalValue}
            />
          </View>
          <View
            style={{
              flexDirection: 'column',
              alignSelf: 'stretch',
              justifyContent: 'center',
              padding: 20,
            }}>
            <Text>
              {this.state.snapToStartValue
                ? 'SnapToStart On '
                : 'SnapToStart Off '}
            </Text>
            <Switch
              onValueChange={this.toggleSwitch2}
              value={this.state.snapToStartValue}
            />
          </View>
          <View
            style={{
              flexDirection: 'column',
              alignSelf: 'stretch',
              justifyContent: 'center',
              padding: 20,
            }}>
            <Text>
              {this.state.snapToEndValue ? 'SnapToEnd On ' : 'SnapToEnd Off '}
            </Text>
            <Switch
              onValueChange={this.toggleSwitch3}
              value={this.state.snapToEndValue}
            />
          </View>
          <View
            style={{
              flexDirection: 'column',
              alignSelf: 'stretch',
              justifyContent: 'center',
              padding: 20,
            }}>
            <Text>
              {this.state.zoomValue ? 'Zoom 2.0 on ' : 'Zoom 2.0 Off '}
            </Text>
            <Switch
              onValueChange={this.toggleSwitch4}
              value={this.state.zoomValue}
            />
          </View>
          <View
            style={{
              flexDirection: 'column',
              alignSelf: 'stretch',
              justifyContent: 'center',
              padding: 20,
            }}>
            <Text>
              {this.state.alignToStartValue ? 'AlignToStart' : 'AlignToEnd'}
            </Text>
            <Switch
              onValueChange={this.toggleSwitch5}
              value={this.state.alignToStartValue}
            />
          </View>
          <View
            style={{
              flexDirection: 'column',
              alignSelf: 'stretch',
              justifyContent: 'center',
              padding: 20,
            }}>
            <Text>
              {this.state.snapToOffsets
                ? 'SnapToOffsets[100,500]'
                : 'SnapToOffsets[null]'}
            </Text>
            <Switch
              onValueChange={this.toggleSwitch6}
              value={this.state.snapToOffsets}
            />
          </View>
          <View
            style={{
              flexDirection: 'column',
              alignSelf: 'stretch',
              justifyContent: 'center',
              padding: 20,
            }}>
            <Text>
              {this.state.pagingEnabled
                ? 'pagingEnabled on'
                : 'pagingEnabled off'}
            </Text>
            <Switch
              onValueChange={this.toggleSwitch7}
              value={this.state.pagingEnabled}
            />
          </View>
        </View>
        <View style={{flex: 0.8, alignSelf: 'center', flexDirection: 'column'}}>
          <ScrollView
            style={
              this.state.horizontalValue
                ? styles.horizontalScrollViewStyle
                : styles.verticalScrollViewStyle
            }
            refreshControl={
              <RefreshControl
                refreshing={this.state.refreshing}
                onRefresh={this.onRefresh}
              />
            }
            snapToOffsets={
              this.state.snapToOffsets ? [100.0, 500.0] : undefined
            }
            pagingEnabled={this.state.pagingEnabled}
            minimumZoomScale={0.1}
            maximumZoomScale={2.0}
            zoomScale={this.state.zoomValue ? 2.0 : 1.0}
            snapToStart={this.state.snapToStartValue}
            snapToEnd={this.state.snapToEndValue}
            snapToAlignment={this.state.alignToStartValue ? 'start' : 'end'}
            horizontal={this.state.horizontalValue}>
            {this.makeItems(20, [styles.itemWrapper])}
          </ScrollView>
        </View>
      </View>
    );

    return item;
  }
}

const styles = StyleSheet.create({
  horizontalScrollViewStyle: {
    padding: 10,
    width: 500,
    height: 120,
  },
  verticalScrollViewStyle: {
    padding: 10,
    width: 120,
    height: 500,
  },
  itemWrapper: {
    backgroundColor: '#dddddd',
    alignItems: 'center',
    borderRadius: 5,
    borderWidth: 5,
    borderColor: '#a52a2a',
    padding: 10,
    margin: 5,
    width: 90,
    height: 90,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
