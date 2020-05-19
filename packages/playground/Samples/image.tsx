/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
import {Picker} from '@react-native-community/picker';
import {AppRegistry, Image, View, Text, Switch, StyleSheet} from 'react-native';

const largeImageUri =
  'https://cdn.freebiesupply.com/logos/large/2x/react-logo-png-transparent.png';
const smallImageUri =
  'http://facebook.github.io/react-native/img/header_logo.png';

const dataImageUri =
  'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAzCAYAAAA6oTAqAAAAEXRFWHRTb2Z0d2FyZQBwbmdjcnVzaEB1SfMAAABQSURBVGje7dSxCQBACARB+2/ab8BEeQNhFi6WSYzYLYudDQYGBgYGBgYGBgYGBgYGBgZmcvDqYGBgmhivGQYGBgYGBgYGBgYGBgYGBgbmQw+P/eMrC5UTVAAAAABJRU5ErkJggg==';

export default class Bootstrap extends React.Component<
  {},
  {
    selectedResizeMode:
      | 'center'
      | 'stretch'
      | 'cover'
      | 'contain'
      | 'repeat'
      | undefined;
    includeBorder: boolean;
    selectedSource: string;
    imageUri: string;
  }
> {
  state = {
    selectedResizeMode: 'center' as 'center',
    selectedSource: 'small',
    includeBorder: false,
    imageUri: 'http://facebook.github.io/react-native/img/header_logo.png',
  };

  switchImageUri = (value: string) => {
    this.setState({selectedSource: value});

    let imageUri = '';

    if (value === 'small') {
      imageUri = smallImageUri;
    } else if (value === 'large') {
      imageUri = largeImageUri;
    } else if (value === 'data') {
      imageUri = dataImageUri;
    }

    this.setState({imageUri});
  };

  render() {
    return (
      <View style={styles.container}>
        <View style={styles.rowContainer}>
          <Text style={styles.title}>ResizeMode</Text>
          <Picker
            style={{width: 125}}
            selectedValue={this.state.selectedResizeMode}
            onValueChange={value => this.setState({
              selectedResizeMode: value as | 'center'
                | 'stretch'
                | 'cover'
                | 'contain'
                | 'repeat'
                | undefined
            })}>
            <Picker.Item label="cover" value="cover" />
            <Picker.Item label="contain" value="contain" />
            <Picker.Item label="stretch" value="stretch" />
            <Picker.Item label="center" value="center" />
            <Picker.Item label="repeat" value="repeat" />
          </Picker>
        </View>
        <View style={styles.rowContainer}>
          <Text style={styles.title}>Image Source</Text>
          <Picker
            style={{width: 125}}
            selectedValue={this.state.selectedSource}
            onValueChange={value => this.switchImageUri(value)}>
            <Picker.Item label="small" value="small" />
            <Picker.Item label="large" value="large" />
            <Picker.Item label="data" value="data" />
            <Picker.Item label="svg" value="svg" />
          </Picker>
        </View>
        <View style={styles.rowContainer}>
          <Text>No Border</Text>
          <Switch
            style={{marginLeft: 10}}
            value={this.state.includeBorder}
            onValueChange={(value: boolean) =>
              this.setState({includeBorder: value})
            }
          />
          <Text>Round Border</Text>
        </View>
        <View style={styles.imageContainer}>
          <Image
            style={
              this.state.includeBorder ? styles.imageWithBorder : styles.image
            }
            source={
              this.state.selectedSource === 'svg'
                ? require('../Samples/images/Microsoft-Logo.svg')
                : {uri: this.state.imageUri}
            }
            resizeMode={this.state.selectedResizeMode}
          />
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    marginBottom: 5,
  },
  rowContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    marginBottom: 5,
  },
  imageContainer: {
    marginTop: 5,
    backgroundColor: 'orange',
    height: '50%',
    width: '75%',
  },
  image: {
    height: '100%',
    width: '100%',
  },
  imageWithBorder: {
    height: '100%',
    width: '100%',
    borderRadius: 10.0,
    borderWidth: 10,
    borderColor: 'green',
    backgroundColor: 'red',
  },
  title: {
    fontWeight: 'bold',
    margin: 5,
    width: 100,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
