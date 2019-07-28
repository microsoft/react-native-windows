/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
import {Picker} from 'react-native-windows';
import {AppRegistry, Image, View, Text, Switch, StyleSheet} from 'react-native';

const largeImageUri =
  'https://cdn.freebiesupply.com/logos/large/2x/react-logo-png-transparent.png';
const smallImageUri =
  'http://facebook.github.io/react-native/img/header_logo.png';

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
    useLargeImage: boolean;
    imageUrl: string;
  }
> {
  state = {
    selectedResizeMode: 'center' as 'center',
    useLargeImage: false,
    imageUrl: 'http://facebook.github.io/react-native/img/header_logo.png',
  };

  switchImageUrl = () => {
    const useLargeImage = !this.state.useLargeImage;
    this.setState({useLargeImage});

    const imageUrl = useLargeImage ? largeImageUri : smallImageUri;
    this.setState({imageUrl});
  };

  render() {
    return (
      <View style={styles.container}>
        <View style={styles.rowContainer}>
          <View style={styles.rowContainer}>
            <Text style={styles.title}>ResizeMode</Text>
            <Picker
              style={{width: 125}}
              selectedValue={this.state.selectedResizeMode}
              onValueChange={value =>
                this.setState({selectedResizeMode: value})
              }>
              <Picker.Item label="cover" value="cover" />
              <Picker.Item label="contain" value="contain" />
              <Picker.Item label="stretch" value="stretch" />
              <Picker.Item label="center" value="center" />
              <Picker.Item label="repeat" value="repeat" />
            </Picker>
          </View>
          <View style={styles.rowContainer}>
            <Text style={styles.title}>Image Size</Text>
            <Text style={{marginRight: 5}}>Small</Text>
            <Switch
              value={this.state.useLargeImage}
              onValueChange={this.switchImageUrl}
            />
            <Text style={{marginRight: 5}}>Large</Text>
          </View>
        </View>
        <View style={styles.imageContainer}>
          <Image
            style={styles.image}
            source={{uri: this.state.imageUrl}}
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
  title: {
    fontWeight: 'bold',
    margin: 5,
    width: 80,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
