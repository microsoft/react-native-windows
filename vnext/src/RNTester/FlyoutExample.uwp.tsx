/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Button, Text, TextInput, View} from 'react-native';
import {CheckBox, Flyout, Picker} from '../index.uwp';
import {Placement} from '../Libraries/Components/Flyout/FlyoutProps';

interface IFlyoutExampleState {
  isFlyoutVisible: boolean;
  isFlyoutTwoVisible: boolean;
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
  'right-edge-aligned-bottom',
];

class FlyoutExample extends React.Component<{}, IFlyoutExampleState> {
  // tslint:disable-next-line:no-any
  private _anchor: any;
  private _anchorTwo: any;

  public state: IFlyoutExampleState = {
    isFlyoutVisible: false,
    isFlyoutTwoVisible: false,
    buttonTitle: 'Open Flyout',
    isLightDismissEnabled: true,
    popupCheckBoxState: true,
    placementOptions: 'top',
  };

  public constructor(props: any) {
    super(props);
    this._anchor = React.createRef();
    this._anchorTwo = React.createRef();
  }

  public render() {
    return (
      <View>
        <View style={{flexDirection: 'row', paddingTop: 20}}>
          <Text style={{padding: 10}}>Placement Options: </Text>
          <Picker
            style={{width: 200, height: 35}}
            selectedValue={this.state.placementOptions}
            onValueChange={value => this.setState({placementOptions: value})}>
            {placementValues.map(item => (
              <Picker.Item key={item} label={item} value={item} />
            ))}
          </Picker>
        </View>
        <View style={{justifyContent: 'center', padding: 20, width: 200}}>
          <Button onPress={this._onPress} title={this.state.buttonTitle} />
        </View>
        <View style={{flexDirection: 'row', paddingTop: 200}}>
          <Text style={{padding: 10, width: 300, height: 32}}>
            Text Input to Anchor flyout to:{' '}
          </Text>
          <TextInput style={{height: 32, width: 250}} ref={this._setRef} />
        </View>
        {this.state.isFlyoutVisible && (
          <Flyout
            isOpen={this.state.isFlyoutVisible}
            isLightDismissEnabled={this.state.isLightDismissEnabled}
            onDismiss={this._onFlyoutDismissed}
            target={this._anchor}
            placement={this.state.placementOptions}>
            <View
              style={{backgroundColor: 'lightgray', width: 300, height: 400}}>
              <Text
                style={{
                  justifyContent: 'center',
                  paddingTop: 10,
                  paddingBottom: 30,
                }}>
                This is a flyout
              </Text>
              <Button onPress={this._onFlyoutButtonPressed} title="Close" />
              <View style={{flexDirection: 'row'}}>
                <Text style={{padding: 10}}>isLightDismissEnabled: </Text>
                <CheckBox
                  style={{justifyContent: 'center', padding: 20}}
                  checked={this.state.popupCheckBoxState}
                  onValueChange={value =>
                    this.setState({
                      popupCheckBoxState: value,
                      isLightDismissEnabled: value,
                    })
                  }
                />
              </View>
              <TextInput style={{height: 32, width: 100}} />
              <Text
                style={{
                  justifyContent: 'center',
                  paddingTop: 10,
                  marginLeft: 50
                }}>
                Placement Options
              </Text>
              <Picker
                style={{borderBottomWidth: 1,
                  borderTopWidth: 1,
                  borderLeftWidth: 1,
                  borderRightWidth: 1,
                  width: 200,
                  marginLeft: 50,
               } } 
                selectedValue={this.state.placementOptions}
                onValueChange={value => this.setState({placementOptions: value})}
              >
                { placementValues.map(item => <Picker.Item key={item} label={item} value={item} /> ) }
              </Picker>
              <View
                style={{
                  width: 150,
                  marginLeft: 75,
                  marginTop: 10
                }}>
                  <Button onPress={this._onPressTwo} title={'Open Another Flyout'}  ref={this._setRefTwo} />
                </View>
            </View>
          </Flyout>
        )}
        {this.state.isFlyoutTwoVisible && (
          <Flyout
            isOpen={this.state.isFlyoutTwoVisible}
            isLightDismissEnabled={true}
            onDismiss={this._onFlyoutTwoDismissed}
            target={this._anchorTwo}
            placement={this.state.placementOptions}>
            <View
              style={{backgroundColor: 'lightblue', width: 200, height: 300}}>
              <Text>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed dapibus felis eget augue condimentum suscipit. Suspendisse hendrerit, libero aliquet malesuada tempor, urna nibh consectetur tellus, vitae efficitur quam erat non mi. Maecenas vitae eros sit amet quam vestibulum porta sed sit amet tellus. Fusce quis lectus congue, fringilla arcu id, luctus urna. Cras sagittis ornare mauris sit amet dictum. Vestibulum feugiat laoreet fringilla. Vivamus ac diam vehicula felis venenatis sagittis vitae ultrices elit. Curabitur libero augue, laoreet quis orci vitae, congue euismod massa. Aenean nec odio sed urna vehicula fermentum non a magna. Quisque ut commodo neque, eget eleifend odio. Sed sit amet lacinia sem. Suspendisse in metus in purus scelerisque vestibulum. Nam metus dui, efficitur nec metus non, tincidunt pharetra sapien. Praesent id convallis metus, ut malesuada arcu. Quisque quam libero, pharetra eu tellus ac, aliquam fringilla erat. Quisque tempus in lorem ac suscipit.</Text>
            </View>
          </Flyout>
        )}
      </View>
    );
  }

  private _setRef = (textInput: TextInput) => {
    this._anchor = textInput;
  };

  _onPress = () => {
    this.setState({buttonTitle: 'Close Flyout', isFlyoutVisible: true});
  };

  _onFlyoutButtonPressed = () => {
    this.setState({buttonTitle: 'Open Flyout', isFlyoutVisible: false});
  };

  _onFlyoutDismissed = (_isOpen: boolean) => {
    this.setState({isFlyoutVisible: false});
    this.setState({isFlyoutTwoVisible: false});
    this.setState({buttonTitle: 'Open Flyout'});
  };

  _setRefTwo = (element: any) => {
    this._anchorTwo = element;
  };

  _onPressTwo = () => {
    this.setState({isFlyoutTwoVisible: true});
  };

  _onFlyoutTwoDismissed = (_isOpen: boolean) => {
    this.setState({isFlyoutTwoVisible: false});
  };

}

export const displayName = (_undefined?: string) => {};
export const title = '<Flyout>';
export const description =
  'Displays content on top of existing content, within the bounds of the application window.';
export const examples = [
  {
    title: 'Flyout Anchor to text input',
    render: function(): JSX.Element {
      return <FlyoutExample />;
    },
  },
];
