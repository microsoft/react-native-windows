// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Button, Text, TextInput, View } from 'react-native';
import { CheckBox, Flyout, Picker } from '../../src/index.uwp';
import { Placement as Placement } from '../../src/Libraries/Components/Flyout/FlyoutProps';

interface IFlyoutExampleState {
  isFlyoutVisible: boolean;
  buttonTitle: string;
  isLightDismissEnabled: boolean;
  popupCheckBoxState: boolean;
  placementOptions: Placement;
}

class FlyoutExample extends React.Component<{}, IFlyoutExampleState> {

  // tslint:disable-next-line:no-any
  private _textInput: any;

  public state: IFlyoutExampleState = {
    isFlyoutVisible: false,
    buttonTitle: 'Open Flyout',
    isLightDismissEnabled: true,
    popupCheckBoxState: true,
    placementOptions: Placement.top,
  };

  public constructor(props: any) {
    super(props);
    this._textInput = React.createRef();
  }

  public render() {
    let placementValues = (Object as any)["values"](Placement) as string[]; 
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
          <Button onPress={ this._onPress } title={ this.state.buttonTitle }></Button>
        </View>
        <View style={ { flexDirection: 'row', paddingTop: 20 } }>
          <Text
            style={ { padding: 10, width: 300, height: 32 } }>Text Input to Anchor flyout to: </Text>
          <TextInput
            style={ { height: 32, width: 300 } }
            ref={ this._setRef }
          />
        </View>
        { this.state.isFlyoutVisible && (
          <Flyout
            isOpen={ this.state.isFlyoutVisible }
            isLightDismissEnabled={ this.state.isLightDismissEnabled }
            onDismiss={ this._onFlyoutDismissed }
            target={ this._textInput }
            placement= {this.state.placementOptions}
          >
            <View style={ { backgroundColor: 'lightgray', width: 200, height: 300 } }>
              <Text style={ { justifyContent: 'center', paddingTop: 10, paddingBottom: 30 } }>This is a flyout</Text>
              <Button onPress={ this._onFlyoutButtonPressed } title="Close" />
              <View style={ { flexDirection: 'row' } }>
                <Text style={ { padding: 10 } }>isLightDismissEnabled: </Text>
                <CheckBox
                    style={{ justifyContent: 'center', padding: 20 }}
                    checked={this.state.popupCheckBoxState}
                    onValueChange={value => this.setState({ popupCheckBoxState: value, isLightDismissEnabled: value })} />
              </View>
              <TextInput style={ { height: 32, width: 100 } } />
            </View>
          </Flyout> 
        )}
      </View>
    );
  }

  private _setRef = (textInput: TextInput) => {
    this._textInput = textInput;
  }

  _onPress = () => {
    this.setState({ buttonTitle: 'Close Flyout', isFlyoutVisible: true });
  }

  _onFlyoutButtonPressed = () => {
    this.setState({ buttonTitle: 'Open Flyout', isFlyoutVisible: false });
  }

  _onFlyoutDismissed = (isOpen: boolean) => {
    this.setState({ isFlyoutVisible: false });
    this.setState({ buttonTitle: 'Open Flyout' });
  }
}


export const displayName = (_undefined?: string) => { };
export const title = '<Flyout>';
export const description = 'Displays content on top of existing content, within the bounds of the application window.';
export const examples = [
  {
    title: 'Flyout Anchor to text input',
    render: function (): JSX.Element {
      return <FlyoutExample />;
    },
  }
];
