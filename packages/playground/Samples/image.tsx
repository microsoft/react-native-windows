/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {
  AppRegistry,
  Image,
  View,
  Text,
  Switch,
  StyleSheet,
  TouchableOpacity,
  Modal,
  FlatList,
} from 'react-native';

const loadingImageUri = require('..\\Samples\\images\\loading.png');

const largeImageUri =
  'https://cdn.freebiesupply.com/logos/large/2x/react-logo-png-transparent.png';

const smallImageUri =
  'https://cdn.pixabay.com/photo/2021/08/02/00/10/flowers-6515538_1280.jpg';

const reactLogoUri = 'https://reactjs.org/logo-og.png';

const svgUri =
  'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAxMDAgMTAwIj48Y2lyY2xlIGN4PSI1MCIgY3k9IjUwIiByPSI0MCIgZmlsbD0iIzYxREFGQiIvPjwvc3ZnPg==';

const dataImageUri =
  'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAzCAYAAAA6oTAqAAAAEXRFWHRTb2Z0d2FyZQBwbmdjcnVzaEB1SfMAAABQSURBVGje7dSxCQBACARB+2/ab8BEeQNhFi6WSYzYLYudDQYGBgYGBgYGBgYGBgYGBgZmcvDqYGBgmhivGQYGBgYGBgYGBgYGBgYGBgbmQw+P/eMrC5UTVAAAAABJRU5ErkJggg==';

const dataImageSvg =
  'data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4gPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyOCAyOCIgZmlsbD0ibm9uZSI+PHBhdGggZD0iTTEzLjEyNSAwSDBWMTMuMTI1SDEzLjEyNVYwWiIgZmlsbD0iI0YyNTAyMiI+PC9wYXRoPjxwYXRoIGQ9Ik0yOCAwSDE0Ljg3NVYxMy4xMjVIMjhWMFoiIGZpbGw9IiM3RkJBMDAiPjwvcGF0aD48cGF0aCBkPSJNMTMuMTI1IDE0Ljg3NUgwVjI4SDEzLjEyNVYxNC44NzVaIiBmaWxsPSIjMDBBNEVGIj48L3BhdGg+PHBhdGggZD0iTTI4IDE0Ljg3NUgxNC44NzVWMjhIMjhWMTQuODc1WiIgZmlsbD0iI0ZGQjkwMCI+PC9wYXRoPjwvc3ZnPiA=';

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
    blurRadius: number;
    selectedSource: string;
    imageUri: string;
    tintColor: string;
    modalVisible: boolean;
    currentPicker: string | null;
    defaultImageUri: string;
  }
> {
  state = {
    selectedResizeMode: 'center' as 'center',
    selectedSource: 'small',
    includeBorder: false,
    tintColor: 'transparent',
    blurRadius: 0,
    imageUri: smallImageUri,
    modalVisible: false,
    currentPicker: '',
    defaultImageUri: reactLogoUri,
  };

  switchImageUri = (value: string) => {
    this.setState({selectedSource: value});

    let imageUri = '';
    if (value === 'small') {
      imageUri = smallImageUri;
    } else if (value === 'large') {
      imageUri = largeImageUri;
    } else if (value === 'data-svg') {
      imageUri = dataImageSvg;
    } else if (value === 'data') {
      imageUri = dataImageUri;
    } else if (value === 'svg') {
      imageUri = svgUri;
    } else if (value === 'react-logo') {
      imageUri = reactLogoUri;
    }
    this.setState({imageUri});
  };

  setModalVisible = (visible: boolean, pickerName: string | null = null) => {
    this.setState({modalVisible: visible, currentPicker: pickerName});
  };

  setSelection = (value: any) => {
    const {currentPicker} = this.state;
    switch (currentPicker) {
      case 'resizeMode':
        this.setState({selectedResizeMode: value});
        break;
      case 'imageSource':
        this.switchImageUri(value);
        break;
      case 'blurRadius':
        this.setState({blurRadius: value});
        break;
      case 'tintColor':
        this.setState({tintColor: value});
        break;
      default:
        break;
    }

    this.setModalVisible(false);
  };

  renderPickerItems = (options: {label: string; value: any}[]) => {
    return (
      <FlatList
        style={styles.flatList}
        data={options}
        renderItem={({item}) => (
          <TouchableOpacity
            style={styles.item}
            onPress={() => this.setSelection(item.value)}>
            <Text style={styles.itemText}>{item.label}</Text>
          </TouchableOpacity>
        )}
        keyExtractor={item => item.value}
      />
    );
  };

  render() {
    const resizeModes = [
      {label: 'center', value: 'center'},

      {label: 'cover', value: 'cover'},
      {label: 'contain', value: 'contain'},
      {label: 'stretch', value: 'stretch'},
      {label: 'repeat', value: 'repeat'},
    ];

    const imageSources = [
      {label: 'small', value: 'small'},
      {label: 'large', value: 'large'},
      {label: 'data', value: 'data'},
      {label: 'data-svg', value: 'data-svg'},
      {label: 'svg', value: 'svg'},
      {label: 'react-logo', value: 'react-logo'},
    ];

    const blurRadiusOptions = [
      {label: '0', value: 0},
      {label: '5', value: 5},
      {label: '10', value: 10},
    ];

    const tintColors = [
      {label: 'None', value: 'transparent'},
      {label: 'Purple', value: 'purple'},
      {label: 'Green', value: 'green'},
      {label: 'SystemAccentColor', value: 'platformcolor'},
    ];

    return (
      <View style={styles.container}>
        <View style={styles.rowContainer}>
          <Text style={styles.title}>ResizeMode</Text>
          <TouchableOpacity
            onPress={() => this.setModalVisible(true, 'resizeMode')}>
            <Text>{this.state.selectedResizeMode}</Text>
          </TouchableOpacity>
        </View>

        <View style={styles.rowContainer}>
          <Text style={styles.title}>Image Source</Text>
          <TouchableOpacity
            onPress={() => this.setModalVisible(true, 'imageSource')}>
            <Text>{this.state.selectedSource}</Text>
          </TouchableOpacity>
        </View>

        <View style={styles.rowContainer}>
          <Text style={styles.title}>Blur Radius</Text>
          <TouchableOpacity
            onPress={() => this.setModalVisible(true, 'blurRadius')}>
            <Text>{this.state.blurRadius}</Text>
          </TouchableOpacity>
        </View>

        <View style={styles.rowContainer}>
          <Text style={styles.title}>Tint Color</Text>
          <TouchableOpacity
            onPress={() => this.setModalVisible(true, 'tintColor')}>
            <Text>{this.state.tintColor}</Text>
          </TouchableOpacity>
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
            style={[
              styles.image,
              this.state.includeBorder ? styles.imageWithBorder : {},
              this.state.tintColor === 'platformcolor'
                ? styles.imageWithPlatformColor
                : {tintColor: this.state.tintColor},
            ]}
            defaultSource={{uri: this.state.defaultImageUri}}
            source={{uri: this.state.imageUri}}
            loadingIndicatorSource={{uri: loadingImageUri}}
            resizeMode={this.state.selectedResizeMode}
            blurRadius={this.state.blurRadius}
            onLoad={() => console.log('onLoad')}
            onLoadStart={() => console.log('onLoadStart')}
            onLoadEnd={() => console.log('onLoadEnd')}
          />
        </View>

        <Modal
          visible={this.state.modalVisible}
          transparent={true}
          animationType="slide"
          onRequestClose={() => this.setModalVisible(false)}>
          <View>
            {this.state.currentPicker === 'resizeMode' &&
              this.renderPickerItems(resizeModes)}
            {this.state.currentPicker === 'imageSource' &&
              this.renderPickerItems(imageSources)}
            {this.state.currentPicker === 'blurRadius' &&
              this.renderPickerItems(blurRadiusOptions)}
            {this.state.currentPicker === 'tintColor' &&
              this.renderPickerItems(tintColors)}
            <TouchableOpacity
              style={styles.closeButton}
              onPress={() => this.setModalVisible(false)}>
              <Text style={styles.closeText}>Close</Text>
            </TouchableOpacity>
          </View>
        </Modal>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  closeButton: {
    backgroundColor: 'red',
    paddingVertical: 12,
    paddingHorizontal: 20,
    borderRadius: 5,
    alignItems: 'center',
    justifyContent: 'center',
    marginTop: 20,
    alignSelf: 'center',
  },
  closeText: {
    color: '#fff',
    fontSize: 16,
    fontWeight: 'bold',
  },
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    marginBottom: 5,
  },
  flatList: {
    width: '100%',
    flexGrow: 1,
    maxHeight: 300,
  },
  rowContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    marginBottom: 5,
  },
  picker: {
    width: 175,
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
    borderRadius: 10.0,
    borderWidth: 10,
    borderColor: 'green',
    backgroundColor: 'red',
  },
  imageWithPlatformColor: {
    tintColor: 'green',
  },
  loading: {
    height: '10%',
    width: '10%',
  },
  item: {
    paddingVertical: 12,
    paddingHorizontal: 15,
    borderBottomWidth: 1,
    borderBottomColor: '#ddd',
    alignItems: 'flex-start',
  },
  itemText: {
    fontSize: 16,
    color: '#333',
  },
  title: {
    fontWeight: 'bold',
    margin: 5,
    width: 100,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
