/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Button, Text, View, TextInput} from 'react-native';
import {CheckBox, Flyout, Picker} from '../index.uwp';
import {Placement} from '../Libraries/Components/Flyout/FlyoutProps';

interface IContextFlyoutExampleState {
  isFlyoutVisible: boolean;
  buttonTitle: string;
  attachAsContextMenu: boolean;
  popupCheckBoxState: boolean;
  placementOptions: Placement;
  target?: React.ReactNode;
  openMessage?: string;
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

class ContextFlyoutExample extends React.Component<
  {},
  IContextFlyoutExampleState
> {
  // tslint:disable-next-line:no-any
  // private _target: React.ReactNode;

  public state: IContextFlyoutExampleState = {
    isFlyoutVisible: false,
    buttonTitle: 'Open Flyout',
    attachAsContextMenu: true,
    popupCheckBoxState: true,
    placementOptions: 'right-edge-aligned-top',
    target: undefined,
    openMessage: 'closed',
  };

  public constructor(props: any) {
    super(props);
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
          <Button
            onPress={this._onPress}
            title={this.state.buttonTitle}
            ref={ref => {
              if (!this.state.target) {
                this.setState({target: ref});
              }
            }}
          />
        </View>
        <View style={{flexDirection: 'row'}}>
          <Text style={{padding: 10}}>isAttachedAsContextMenu: </Text>
          <CheckBox
            style={{justifyContent: 'center', padding: 20}}
            checked={this.state.popupCheckBoxState}
            onValueChange={value =>
              this.setState({
                popupCheckBoxState: value,
                attachAsContextMenu: value,
                isFlyoutVisible: false,
              })
            }
          />
          <Text style={{padding: 10}}>
            When enabled, the flyout will be assigned to the button's
            ContextFlyout property and will open on right click, context
            shortcut etc.
          </Text>
        </View>
        {this.state.target && (
          <Flyout
            isOpen={this.state.isFlyoutVisible}
            isContextFlyout={this.state.attachAsContextMenu}
            onDismiss={this._onFlyoutDismissed}
            target={this.state.target}
            placement={this.state.placementOptions}
            onOpen={this._onOpen}>
            <View
              style={{backgroundColor: 'lightgray', width: 200, height: 300}}>
              <Text
                style={{
                  justifyContent: 'center',
                  paddingTop: 10,
                  paddingBottom: 30,
                }}>
                This is a flyout. This can be used as a context menu.
              </Text>
              {!this.state.attachAsContextMenu && (
                <Button onPress={this._onFlyoutButtonPressed} title="Close" />
              )}
              <TextInput style={{height: 32, width: 100}} />
            </View>
          </Flyout>
        )}
        <Text style={{padding: 10}}>
          {'Flyout state: ' + this.state.openMessage}
        </Text>
      </View>
    );
  }

  _onPress = () => {
    if (!this.state.attachAsContextMenu) {
      this.setState({buttonTitle: 'Close Flyout', isFlyoutVisible: true});
    }
  };

  _onFlyoutButtonPressed = () => {
    this.setState({buttonTitle: 'Open Flyout', isFlyoutVisible: false});
  };

  _onFlyoutDismissed = (isOpen: boolean) => {
    this.setState({isFlyoutVisible: isOpen});
    this.setState({buttonTitle: 'Open Flyout', openMessage: 'dismissed'});
  };
  _onOpen = () => {
    this.setState({openMessage: 'Open'});
  };
}

export const displayName = (_undefined?: string) => {};
export const title = '<Flyout> isContextFlyout Example';
export const description =
  'Example of Flyout Assigned to ContextFlyout property of a UIElement';
export const examples = [
  {
    title: 'Flyout Anchor to button as context menu',
    render: function(): JSX.Element {
      return <ContextFlyoutExample />;
    },
  },
];
