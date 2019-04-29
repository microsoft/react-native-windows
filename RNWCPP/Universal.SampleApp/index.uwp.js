// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

import React, { Component } from 'react';
import {
  AppRegistry,
  Button,
  StyleSheet,
  ScrollView,
  Switch,
  Text,
  View,
  WebView,
  Image,
  TextInput,
  TouchableHighlight,
  ActivityIndicator
} from 'react-native';
import {
  CheckBox,
  DatePicker,
  Popup,
  Picker
} from 'react-native-windows';

class TicTacButton extends Component {
  constructor(props) {
    super(props);
    this._onPress = this._onPress.bind(this);
    this.state = { text: '?' };
  }

  render() {
    return (
      <Button onPress={this._onPress} title={this.state.text} accessibilityLabel={this.state.text} />
    );
  }

  _onPress(e) {
    if (this.state.text == 'X')
      this.setState({ text: 'o' });
    else
      this.setState({ text: 'X' });
  }
}

class PopupButton extends Component {
  constructor(props) {
    super(props);
    this._onPress = this._onPress.bind(this);
    this._onPopupButtonPressed = this._onPopupButtonPressed.bind(this);
    this._onPopupDismissed = this._onPopupDismissed.bind(this);
    this.state = {
      buttonTitle: 'Open Flyout',
      isFlyoutVisible: false,
      popupCheckBoxState: true,
      isLightDismissEnabled: false
    };
  }

  render() {
    return (
      <View style={{ flexDirection: 'row', padding: 20 }}>
        <Text style={{ padding: 5 }}>isLightDismissEnabled: </Text>
        <CheckBox
          checked={this.state.isLightDismissEnabled}
          onValueChange={value => this.setState({ isLightDismissEnabled: value })} />
        <Button onPress={this._onPress} title={this.state.buttonTitle} accessibilityLabel={this.state.buttonTitle}/>
        <Popup
          isOpen={this.state.isFlyoutVisible}
          isLightDismissEnabled={this.state.isLightDismissEnabled}
          verticalOffset={50}
          horizontalOffset={350}
          onDismiss={this._onPopupDismissed}
        >
          <View style={{ backgroundColor: 'lightgray', width: 200, height: 300 }}>
            <Text style={{ justifyContent: 'center', paddingTop: 10 }}>This is a flyout</Text>
            <CheckBox
              style={{ justifyContent: 'center', padding: 20 }}
              checked={this.state.popupCheckBoxState}
              onValueChange={value => this.setState({ popupCheckBoxState: value })} />
            <Button onPress={this._onPopupButtonPressed} title="Close" />
          </View>
        </Popup>
      </View>
    );
  }

  openPopup() {
    this.setState({ buttonTitle: 'Close Flyout', isFlyoutVisible: true });
  }

  closePopup() {
    this.setState({ buttonTitle: 'Open Flyout', isFlyoutVisible: false });
  }

  _onPress(e) {
    if (this.state.isFlyoutVisible)
      this.closePopup();
    else
      this.openPopup();
  }

  _onPopupButtonPressed(e) {
    this.closePopup();
  }

  _onPopupDismissed(event) {
    this.closePopup();
  }
}

export default class Bootstrap extends Component {
  state = {
    checkBoxIsOn: true,
    switchIsOn: true,
    pickerSelectedValue: "key1",
    pickerSelectedIndex: 0,
    datePickerSelectedValue: new Date(),
  };

  render() {
    return (
      <ScrollView>
        <View style={styles.container}>
          <Text style={styles.welcome}>
            Welcome to React Native
        </Text>
          <Text style={styles.instructions}>
            To get started, edit index.uwp.js
        </Text>
        <Text>
          Nested text
          <Text style={{color: '#3333ff'}}> starts right here! <Text style={{color: '#ff3333'}}>(But wait, here is more!)</Text></Text>
        </Text>
        <Text>ScrollView:</Text>
        <ScrollView style={{width: 200, height: 200}}
          onScroll={() => { console.log('onScroll!'); }}
          onScrollBeginDrag={() => { console.log('onScroll begin drag!'); }}
          onScrollEndDrag={() => { console.log('onScroll end drag!'); }}
          >
          <Text>Foo</Text>
          <Text>Foo2</Text>
          <Text>Foo3</Text>
          <Text>Foo4</Text>
          <Text>Foo5</Text>
          <Text>Foo</Text>
          <Text>Foo</Text>
          <Text>Foo</Text>
          <Text>Foo</Text>
          <Text>Foo</Text>
          <View style={{height:500}}>
            <Text>Foo</Text>
          </View>
        </ScrollView>
        <TicTacButton/>
        <ActivityIndicator size='large' color="green"></ActivityIndicator>
        <Text style={{marginTop: 15}}>Big Border & Clipping Tests:</Text>
          <View style={{ flexDirection: 'row' }}>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>accessible</Text>
              <View accessible={true} acceptsKeyboardFocus={true} accessibilityLabel="Empty Rectangle" style={{ backgroundColor: 'yellow', borderWidth: 1, borderColor: 'brown', width: 60, height: 60, margin: 10 }} />
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>no border</Text>
              <View style={{ backgroundColor: 'orange', borderWidth: 4, width: 60, height: 60, margin: 10 }} />
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>no back</Text>
              <View style={{ borderWidth: 4, borderColor: 'black', width: 60, height: 60, margin: 10 }} />
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>normal</Text>
              <View style={{ backgroundColor: 'orange', borderColor: 'black', borderWidth: 4, width: 60, height: 60, margin: 10 }} />
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>no clip</Text>
              <View style={{backgroundColor: 'orange', borderColor: 'black', borderWidth: 4, borderTopStartRadius: 10.0, borderTopEndRadius: 15.0, borderBottomStartRadius: 20.0, borderBottomEndRadius: 25.0, width: 60, height: 60, margin: 10}}>
                <View style={{backgroundColor: 'green', width: 52, height: 52}} />
              </View>
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>no clip</Text>
              <View style={{backgroundColor: 'orange', borderColor: 'black', borderWidth: 4, borderTopStartRadius: 10.0, borderTopEndRadius: 15.0, borderBottomStartRadius: 20.0, borderBottomEndRadius: 25.0, width: 60, height: 60, margin: 10}}>
                <View style={{backgroundColor: 'green', width: 30, height: 15, left: -10, top: 0}}/>
                <View style={{backgroundColor: 'blue', width: 15, height: 40, left: 0, top: 5}}/>
                <View style={{backgroundColor: 'crimson', width: 40, height: 40, left: 25, top: -50}}/>
              </View>
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>no clip</Text>
              <View style={{backgroundColor: 'orange', borderColor: 'black', borderWidth: 4, borderTopStartRadius: 10.0, borderTopEndRadius: 15.0, borderBottomStartRadius: 20.0, borderBottomEndRadius: 25.0, width: 60, height: 60, margin: 10, overflow: 'visible'}}>
                <View style={{backgroundColor: 'green', width: 30, height: 15, left: -10, top: 0}}/>
                <View style={{backgroundColor: 'blue', width: 15, height: 40, left: 0, top: 5}}/>
                <View style={{backgroundColor: 'crimson', width: 40, height: 40, left: 25, top: -50}}/>
              </View>
           </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>clipped</Text>
              <View style={{backgroundColor: 'orange', borderColor: 'black', borderWidth: 4, borderTopStartRadius: 10.0, borderTopEndRadius: 15.0, borderBottomStartRadius: 20.0, borderBottomEndRadius: 25.0, width: 60, height: 60, margin: 10, overflow: 'hidden'}}>
                <View style={{backgroundColor: 'green', width: 30, height: 15, left: -10, top: 0}}/>
                <View style={{backgroundColor: 'blue', width: 15, height: 40, left: 0, top: 5}}/>
                <View style={{backgroundColor: 'crimson', width: 40, height: 40, left: 25, top: -50}}/>
              </View>
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>circle</Text>
              <View style={{backgroundColor: 'orange', borderRadius: 30, width: 60, height: 60, margin: 10, overflow: 'hidden'}}>
                <View style={{backgroundColor: 'purple', width: 60, height: 60}} />
              </View>
            </View>
            <View style={{ flexDirection: 'column', alignItems: 'center' }}>
              <Text>circle</Text>
              <View style={{ backgroundColor: 'orange', borderRadius: 30, width: 60, height: 60, margin: 10 }} acceptsKeyboardFocus={true}>
                <View style={{backgroundColor: 'magenta', width: 60, height: 60}} />
              </View>
            </View>
          </View>
          <Text style={{marginTop: 5}}>Border Tests:</Text>
          <View style={{flexDirection: 'row'}}>
            <View style={{borderColor: 'red', borderWidth: 1, borderTopStartRadius: 5.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderTopEndRadius: 5.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderBottomStartRadius: 5.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderBottomEndRadius: 5.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderRadius: 10.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderRadius: 10.0, borderTopStartRadius: 5.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderRadius: 10.0, borderTopEndRadius: 5.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderRadius: 10.0, borderBottomStartRadius: 5.0, width:40, height:40, margin:4}} />
            <View style={{borderColor: 'red', borderWidth: 1, borderRadius: 10.0, borderBottomEndRadius: 5.0, width:40, height:40, margin:4}} />
          </View>
          <Text style={{margin: 5, marginTop: 10}}>Position Tests:</Text>
          <View style={{width: 500, height: 100, backgroundColor: 'lightblue'}}>
            <View style={{backgroundColor: 'blue', width: 100, height: 40, position: 'absolute', left: '50%'}}/>
            <View style={{backgroundColor: 'red', width: 100, height: 40, position: 'absolute', top: '25%'}}/>
            <View style={{backgroundColor: 'green', width: 100, height: 40, position: 'absolute', bottom: '5%'}}/>
            <View style={{backgroundColor: 'yellow', width: 100, height: 40, position: 'absolute', right: '50%'}}/>
            <View style={{backgroundColor: 'aqua', width: '50%', height: 40, position: 'absolute', left: 20, top: 30}}/>
            <View style={{backgroundColor: 'orange', width: 50, height: 50, position: 'absolute', right: 10, bottom: 10}}/>
          </View>

          <View style={{ flexDirection: 'row', alignItems: 'center', padding: 10 }}>
            <Picker style={{ width: 100 }} selectedValue={this.state.pickerSelectedValue} onValueChange={this.pickerValueChange} accessibilityLabel="test picker" testID="pickerID">
              <Picker.Item label="item 1" color="blue" value="key0" />
              <Picker.Item label="item 2" value="key1" />
            </Picker>
            <Text style={{ margin: 6 }}>selectedIndex: {this.state.pickerSelectedIndex} selectedValue: {this.state.pickerSelectedValue}</Text>
            <Button title="Clear selection" onPress={this.pickerClearSelection} />
          </View>

          <View style={{ alignItems: 'center', padding: 10 }}>
            <Text>Test Popup: </Text>
            <PopupButton />
          </View>

          <View style={{ backgroundColor: 'blue', marginTop: 15 }}>
            <Image
              style={{ width: 50, height: 50 }}
              source={{ uri: 'http://facebook.github.io/react-native/img/header_logo.png' }}
              onLoadStart={() => { console.log('image onLoadStart!'); }}
              onLoad={() => { console.log('image onLoad!'); }}
              onLoadEnd={() => { console.log('image onLoadEnd!'); }}
              onError={() => { console.log('image onError!'); }}
            />
          </View>
          <View style={{ flexDirection: 'row', alignItems: 'center', marginTop: 15 }}>
            <CheckBox
              onValueChange={value => this.setState({ checkBoxIsOn: value })}
              defaultChecked={true}
              checked={this.state.checkBoxIsOn} />
            <Text>Checkbox {this.state.checkBoxIsOn ? "ON" : "OFF"}</Text>
          </View>
          <View style={{ flexDirection: 'row', alignItems: 'center', marginTop: 15 }}>
            <Switch
              onValueChange={value => this.setState({ switchIsOn: value })}
              defaultChecked={true}
              value={this.state.switchIsOn} />
            <Text>Switch {this.state.switchIsOn ? "ON" : "OFF"}</Text>
          </View>
          <View style={{ padding: 10 }}>
            <View>
              <TouchableHighlight style={{ height: 30 }} onPress={this.focusTextInputPressed} underlayColor={'transparent'}>
                <View style={[this.state.highlightPressed ? styles.selected : {}]}>
                  <Text>Click to focus textbox</Text>
                </View>
              </TouchableHighlight>
            </View>
            <View>
              <TouchableHighlight style={{ height: 30 }} onPress={this.blurTextInputPressed} underlayColor={'transparent'}>
                <View style={[this.state.highlightPressed ? styles.selected : {}]}>
                  <Text>Click to remove focus from textbox</Text>
                </View>
              </TouchableHighlight>
            </View>
            <View>
              <TouchableHighlight style={{ height: 30 }} onPress={this.clearTextInputPressed} underlayColor={'transparent'}>
                <View style={[this.state.highlightPressed ? styles.selected : {}]}>
                  <Text>Click to clear textbox</Text>
                </View>
              </TouchableHighlight>
            </View>
            <TextInput
              ref={c => (this._input = c)}
              defaultValue={"Test"}
              placeholderTextColor={"hotpink"}
              onChangeText={this.changeTextHandler}
              onFocus={this.focusTextInputHandler}
              onBlur={this.blurTextInputHandler}
              onSelectionChange={this.selectionChangeTextInputHandler}
              onEndEditing={this.endEditingTextInputHandler}
              onContentSizeChange={this.contentSizeChangeTextInputHandler}
              selectionColor={"red"}
              clearTextOnFocus={false}
              selectTextOnFocus={true}
              style={{ height: 30 }}
            />
            <TextInput
              ref={c => (this._input = c)}
              defaultValue={"Test"}
              placeholderTextColor={"hotpink"}
              onChangeText={this.changeTextHandler}
              onFocus={this.focusTextInputHandler}
              onBlur={this.blurTextInputHandler}
              onSelectionChange={this.selectionChangeTextInputHandler}
              onEndEditing={this.endEditingTextInputHandler}
              onContentSizeChange={this.contentSizeChangeTextInputHandler}
              selectionColor={"red"}
              style={{ height: 30 }}
            />
          </View>
          <WebView
            style={{ width: 350, height: 500 }}
            source={{ uri: 'https://login.live.com' }}
          />
          <View style={{ padding: 10 }}>
            <Text>Test DatePicker</Text>
            <Text>Date selected: {this.state.datePickerSelectedValue.toString()}</Text>
            <DatePicker
              placeholderText='select start date'
              dateFormat='longdate'
              dayOfWeekFormat='{dayofweek.abbreviated(3)}'
              style={{ width: 300 }}
              onDateChange={this.datePickerValueChange}
            />
          </View>
        </View>
      </ScrollView>
    );
  }

  throwException = () => {
    throw "This is a test exception";
  }

  mouseEnter = () => {
    this.setState({ mouseEntered: true })
  }

  mouseLeave = () => {
    this.setState({ mouseEntered: false })
  }

  pickerValueChange = (value, index) => {
    this.setState({ pickerSelectedValue: value, pickerSelectedIndex: index });
  }
  pickerClearSelection = () => {
    this.setState({ pickerSelectedValue: undefined, pickerSelectedIndex: -1 });
  }

  highlightTextPressed = () => {
    this.setState({ highlightPressed: !this.state.highlightPressed });

    Linking.canOpenURL("https://www.microsoft.com")
      .then(canOpen => {
        if (canOpen)
          Linking.openURL("https://www.microsoft.com");
      })
      .catch(() => {
        console.error("exception from Linking.canOpenURL");
      })

  }

  focusTextInputPressed = () => {
    this._input.focus();
  }

  blurTextInputPressed = () => {
    this._input.blur();
  }

  clearTextInputPressed = () => {
    this._input.clear();
  }

  changeTextHandler = (text) => {
    console.log(text);
  }

  focusTextInputHandler = () => {
    console.log("textbox focused");
  }

  blurTextInputHandler = () => {
    console.log("textbox blurred");
  }

  selectionChangeTextInputHandler = (event) => {
    console.log("selection start: " + event.nativeEvent.selection.start + ", end: " + event.nativeEvent.selection.end);
  }

  endEditingTextInputHandler = (event) => {
    console.log(event.nativeEvent.text);
  }

  contentSizeChangeTextInputHandler = (event) => {
    console.log("new size width: " + event.nativeEvent.contentSize.width + ", height: " + event.nativeEvent.contentSize.height);
  }

  datePickerValueChange = (date) => {
    this.setState({ datePickerSelectedValue: date });
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
