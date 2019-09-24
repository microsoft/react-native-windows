/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Button, Text, TextInput, View} from 'react-native';
import {Popup} from 'react-native-windows';

interface IPopupExampleState {
  isFlyoutVisible: boolean;
  buttonTitle: string;
  isLightDismissEnabled: boolean;
}

class PopupExample extends React.Component<{}, IPopupExampleState> {
  // tslint:disable-next-line:no-any
  private _textInput: any;

  public state: IPopupExampleState = {
    isFlyoutVisible: false,
    buttonTitle: 'Open Flyout',
    isLightDismissEnabled: false,
  };

  public constructor(props: any) {
    super(props);
    this._textInput = React.createRef();
  }

  public render() {
    return (
      <View>
        <Text style={{width: 250}}>The following tests popup Anchor</Text>
        <View style={{flexDirection: 'row'}}>
          <Text style={{padding: 10, width: 300, height: 32}}>
            Text Input to Anchor popup to:{' '}
          </Text>
          <TextInput style={{height: 32, width: 300}} ref={this._setRef} />
        </View>
        <View style={{justifyContent: 'center', padding: 50}}>
          <Button onPress={this._onPress} title={this.state.buttonTitle} />
        </View>
        {this.state.isFlyoutVisible && (
          <Popup
            isOpen={this.state.isFlyoutVisible}
            isLightDismissEnabled={this.state.isLightDismissEnabled}
            onDismiss={this._onPopupDismissed}
            target={this._textInput}
            horizontalOffset={10}
            verticalOffset={10}>
            <View
              style={{backgroundColor: 'lightgray', width: 200, height: 300}}>
              <Text
                style={{
                  justifyContent: 'center',
                  paddingTop: 10,
                  paddingBottom: 30,
                }}>
                This is a flyout
              </Text>
              <Button onPress={this._onPopupButtonPressed} title="Close" />
            </View>
          </Popup>
        )}
      </View>
    );
  }

  private _setRef = (textInput: TextInput) => {
    this._textInput = textInput;
  };

  _onPress = () => {
    this.setState({buttonTitle: 'Close Flyout', isFlyoutVisible: true});
  };

  _onPopupButtonPressed = () => {
    this.setState({buttonTitle: 'Open Flyout', isFlyoutVisible: false});
  };

  _onPopupDismissed = (isOpen: boolean) => {
    this.setState({buttonTitle: 'Open Flyout', isFlyoutVisible: isOpen});
  };
}

export const displayName = (_undefined?: string) => {};
export const title = '<Popup>';
export const description =
  'Displays content on top of existing content, within the bounds of the application window.';
export const examples = [
  {
    title: 'Popup Anchor to text input',
    render: function(): JSX.Element {
      return <PopupExample />;
    },
  },
];
