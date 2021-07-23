/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import * as React from 'react';
import {
  Modal,
  Picker,
  Platform,
  StyleSheet,
  Switch,
  Text,
  TouchableHighlight,
  View,
  ScrollView,
} from 'react-native';
import type {RNTesterExampleModuleItem} from '../../types/RNTesterTypes';

const Item = Picker.Item;

class Button extends React.Component<$FlowFixMeProps, $FlowFixMeState> {
  state = {
    active: false,
  };

  _onHighlight = () => {
    this.setState({active: true});
  };

  _onUnhighlight = () => {
    this.setState({active: false});
  };

  render() {
    const colorStyle = {
      color: this.state.active ? '#fff' : '#000',
    };
    return (
      <TouchableHighlight
        onHideUnderlay={this._onUnhighlight}
        onPress={this.props.onPress}
        onShowUnderlay={this._onHighlight}
        style={[styles.button, this.props.style]}
        underlayColor="#a9d9d4">
        <Text style={[styles.buttonText, colorStyle]}>
          {this.props.children}
        </Text>
      </TouchableHighlight>
    );
  }
}

const supportedOrientationsPickerValues = [
  ['portrait'],
  ['landscape'],
  ['landscape-left'],
  ['portrait', 'landscape-right'],
  ['portrait', 'landscape'],
  [],
];

class ModalExample extends React.Component<{...}, $FlowFixMe> {
  state: $FlowFixMe = {
    animationType: 'none',
    modalVisible: false,
    transparent: false,
    hardwareAccelerated: false,
    statusBarTranslucent: false,
    presentationStyle: 'fullScreen',
    selectedSupportedOrientation: '0',
    currentOrientation: 'unknown',
    action: '',
  };

  _setModalVisible = visible => {
    this.setState({modalVisible: visible});
  };

  _setAnimationType = type => {
    this.setState({animationType: type});
  };

  _toggleTransparent = () => {
    this.setState({transparent: !this.state.transparent});
  };

  _toggleHardwareAccelerated = () => {
    this.setState({hardwareAccelerated: !this.state.hardwareAccelerated});
  };

  _toggleStatusBarTranslucent = () => {
    this.setState({statusBarTranslucent: !this.state.statusBarTranslucent});
  };

  renderSwitch(): React.Node {
    if (Platform.isTV) {
      return null;
    }
    if (Platform.OS === 'android') {
      return (
        <>
          <Text style={styles.rowTitle}>Hardware Accelerated</Text>
          <Switch
            value={this.state.hardwareAccelerated}
            onValueChange={this._toggleHardwareAccelerated}
          />
          <Text style={styles.rowTitle}>Status Bar Translucent</Text>
          <Switch
            value={this.state.statusBarTranslucent}
            onValueChange={this._toggleStatusBarTranslucent}
          />
          <Text style={styles.rowTitle}>Transparent</Text>
          <Switch
            value={this.state.transparent}
            onValueChange={this._toggleTransparent}
          />
        </>
      );
    }
    return (
      <>
        <Text style={styles.rowTitle}>Transparent</Text>
        <Switch
          value={this.state.transparent}
          onValueChange={this._toggleTransparent}
        />
      </>
    );
  }

  render(): React.Node {
    const modalBackgroundStyle = {
      backgroundColor: this.state.transparent
        ? 'rgba(0, 0, 0, 0.5)'
        : '#f5fcff',
    };
    const innerContainerTransparentStyle = this.state.transparent
      ? {backgroundColor: '#fff', padding: 20}
      : null;
    const activeButtonStyle = {
      backgroundColor: '#ddd',
    };

    return (
      <ScrollView contentContainerStyle={styles.ScrollView}>
        <Modal
          animationType={this.state.animationType}
          presentationStyle={this.state.presentationStyle}
          transparent={this.state.transparent}
          hardwareAccelerated={this.state.hardwareAccelerated}
          statusBarTranslucent={this.state.statusBarTranslucent}
          visible={this.state.modalVisible}
          onRequestClose={() => this._setModalVisible(false)}
          supportedOrientations={
            supportedOrientationsPickerValues[
              Number(this.state.selectedSupportedOrientation)
            ]
          }
          onOrientationChange={evt =>
            this.setState({currentOrientation: evt.nativeEvent.orientation})
          }
          onDismiss={() => {
            if (this.state.action === 'onDismiss') {
              alert(this.state.action);
            }
          }}
          onShow={() => {
            if (this.state.action === 'onShow') {
              alert(this.state.action);
            }
          }}>
          <View style={[styles.container, modalBackgroundStyle]}>
            <View
              style={[styles.innerContainer, innerContainerTransparentStyle]}>
              <Text>
                This modal was presented{' '}
                {this.state.animationType === 'none' ? 'without' : 'with'}{' '}
                animation.
              </Text>
              <Text>
                It is currently displayed in {this.state.currentOrientation}{' '}
                mode.
              </Text>
              <Button
                onPress={this._setModalVisible.bind(this, false)}
                style={styles.modalButton}>
                Close
              </Button>
            </View>
          </View>
        </Modal>
        <View style={styles.row}>
          <Text style={styles.rowTitle}>Animation Type</Text>
          <Button
            onPress={this._setAnimationType.bind(this, 'none')}
            style={
              this.state.animationType === 'none' ? activeButtonStyle : {}
            }>
            none
          </Button>
          <Button
            onPress={this._setAnimationType.bind(this, 'slide')}
            style={
              this.state.animationType === 'slide' ? activeButtonStyle : {}
            }>
            slide
          </Button>
          <Button
            onPress={this._setAnimationType.bind(this, 'fade')}
            style={
              this.state.animationType === 'fade' ? activeButtonStyle : {}
            }>
            fade
          </Button>
        </View>

        <View style={styles.row}>{this.renderSwitch()}</View>
        {this.renderPickers()}
        <Button onPress={this._setModalVisible.bind(this, true)}>
          Present
        </Button>
      </ScrollView>
    );
  }
  renderPickers(): React.Node {
    if (Platform.isTV) {
      return null;
    }
    return (
      <View>
        <View>
          <Text style={styles.rowTitle}>Presentation style</Text>
          <Picker
            selectedValue={this.state.presentationStyle}
            onValueChange={presentationStyle =>
              this.setState({presentationStyle})
            }
            itemStyle={styles.pickerItem}>
            <Item label="Full Screen" value="fullScreen" />
            <Item label="Page Sheet" value="pageSheet" />
            <Item label="Form Sheet" value="formSheet" />
            <Item label="Over Full Screen" value="overFullScreen" />
            <Item label="Default presentationStyle" value="" />
          </Picker>
        </View>

        <View>
          <Text style={styles.rowTitle}>Supported orientations</Text>
          <Picker
            selectedValue={this.state.selectedSupportedOrientation}
            onValueChange={(_, i) =>
              this.setState({selectedSupportedOrientation: i.toString()})
            }
            itemStyle={styles.pickerItem}>
            <Item label="Portrait" value={'0'} />
            <Item label="Landscape" value={'1'} />
            <Item label="Landscape left" value={'2'} />
            <Item label="Portrait and landscape right" value={'3'} />
            <Item label="Portrait and landscape" value={'4'} />
            <Item label="Default supportedOrientations" value={'5'} />
          </Picker>
        </View>

        <View>
          <Text style={styles.rowTitle}>Actions</Text>
          {Platform.OS === 'ios' ? (
            <Picker
              selectedValue={this.state.action}
              onValueChange={action => this.setState({action})}
              itemStyle={styles.pickerItem}>
              <Item label="None" value="" />
              <Item label="On Dismiss" value="onDismiss" />
              <Item label="On Show" value="onShow" />
            </Picker>
          ) : (
            <Picker
              selectedValue={this.state.action}
              onValueChange={action => this.setState({action})}
              itemStyle={styles.pickerItem}>
              <Item label="None" value="" />
              <Item label="On Show" value="onShow" />
            </Picker>
          )}
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    padding: 20,
  },
  innerContainer: {
    borderRadius: 10,
    alignItems: 'center',
  },
  row: {
    alignItems: 'center',
    flex: 1,
    flexDirection: 'row',
    marginBottom: 20,
  },
  rowTitle: {
    flex: 1,
    fontWeight: 'bold',
  },
  button: {
    borderRadius: 5,
    flexGrow: 1,
    height: 44,
    alignSelf: 'stretch',
    justifyContent: 'center',
    overflow: 'hidden',
  },
  buttonText: {
    fontSize: 18,
    margin: 5,
    textAlign: 'center',
  },
  modalButton: {
    marginTop: 10,
  },
  pickerItem: {
    fontSize: 16,
  },
  ScrollView: {
    paddingTop: 10,
    paddingBottom: 100,
  },
});

export default ({
  title: 'Modal Presentation',
  name: 'basic',
  description: 'Modals can be presented with or without animation',
  render: (): React.Node => <ModalExample />,
}: RNTesterExampleModuleItem);
