// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Button, Text, TextInput, View } from 'react-native';
import { MenuFlyout, Picker } from '../../src/index.uwp';
import { Placement  } from '../../src/Libraries/Components/MenuFlyout/MenuFlyoutProps';

interface IMenuFlyoutExampleState {
  isMenuFlyoutVisible: boolean;
  buttonTitle: string;
  isLightDismissEnabled: boolean;
  popupCheckBoxState: boolean;
  placementOptions: Placement;
}

let placementValues: string[] = [
  'top',
  'bottom',
  'left',
  'right',
  'full',
  'top-edge-aligned-left',
  'top-edge-aligned-right',
  'bottom-edge-aligned-left',
  'bottom-edge-aligned-right',
  'left-edge-aligned-top',
  'right-edge-aligned-top',
  'left-edge-aligned-bottom',
  'right-edge-aligned-bottom'
]

class MenuFlyoutExample extends React.Component<{}, IMenuFlyoutExampleState> {

  // tslint:disable-next-line:no-any
  private _textInput: any;

  public state: IMenuFlyoutExampleState = {
    isMenuFlyoutVisible: false,
    buttonTitle: 'Open MenuFlyout',
    isLightDismissEnabled: true,
    popupCheckBoxState: true,
    placementOptions: 'top',
  };

  public constructor(props: any) {
    super(props);
    this._textInput = React.createRef();
  }

  public render() {
    return (
      <View>
        <View style={ { flexDirection: 'row', paddingTop: 20 } }>
          <Text style={ { padding: 10 } }>Placement Options: </Text>
          <Picker
            style={ { width: 200, height: 35 } } selectedValue={ this.state.placementOptions }
            onValueChange={ value => this.setState({ placementOptions: value }) }>
            { placementValues.map(item => <Picker.Item key={item} label={item} value={item} /> ) }
          </Picker>
        </View>
        <View style={ { justifyContent: 'center', padding: 20, width: 200 } }>
          <Button onPress={ this._onPress } title={ this.state.buttonTitle }  ref={ this._setRef }></Button>
        </View>
        <View style={ { flexDirection: 'row', paddingTop: 20 } }>
          <Text
            style={ { padding: 10, width: 300, height: 32 } }>Text Input to Anchor MenuFlyout to: </Text>
          <TextInput
            style={ { height: 32, width: 300 } }
           
          />
        </View>
        { this.state.isMenuFlyoutVisible && (
          <MenuFlyout
            isOpen={ this.state.isMenuFlyoutVisible && false }
            attachAsContextFlyout = {true}
            isLightDismissEnabled={ this.state.isLightDismissEnabled }
            onDismiss={ this._onMenuFlyoutDismissed }
            target={ this._textInput }
            placement= {this.state.placementOptions}
          />
        )}
      </View>
    );
  }

  private _setRef = (textInput: Button) => {
    this._textInput = textInput;
  }

  _onPress = () => {
    this.setState({ buttonTitle: 'Close MenuFlyout', isMenuFlyoutVisible: true });
  }

  _onMenuFlyoutButtonPressed = () => {
    this.setState({ buttonTitle: 'Open MenuFlyout', isMenuFlyoutVisible: false });
  }

  _onMenuFlyoutDismissed = (isOpen: boolean) => {
    this.setState({ isMenuFlyoutVisible: false });
    this.setState({ buttonTitle: 'Open MenuFlyout' });
  }
}


export const displayName = (_undefined?: string) => { };
export const title = '<MenuFlyout>';
export const description = 'Displays content on top of existing content, within the bounds of the application window.';
export const examples = [
  {
    title: 'MenuFlyout Anchor to text input',
    render: function (): JSX.Element {
      return <MenuFlyoutExample />;
    },
  }
];
