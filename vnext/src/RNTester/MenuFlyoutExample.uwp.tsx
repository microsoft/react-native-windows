// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Button, Text, View } from 'react-native';
import { CheckBox, MenuFlyout, Picker, MenuFlyoutItem } from '../../src/index.uwp';
import { Placement  } from '../../src/Libraries/Components/MenuFlyout/MenuFlyoutProps';

interface IMenuFlyoutExampleState {
  isMenuFlyoutVisible: boolean;
  buttonTitle: string;
  attachAsContextMenu: boolean;
  popupCheckBoxState: boolean;
  placementOptions: Placement;
  target?: React.ReactNode;
  message?: string;
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
  // private _target: React.ReactNode;

  public state: IMenuFlyoutExampleState = {
    isMenuFlyoutVisible: false,
    buttonTitle: 'Open MenuFlyout',
    attachAsContextMenu: true,
    popupCheckBoxState: true,
    placementOptions: 'top',
    target: undefined,
    message:"not dismissed"
  };

  public constructor(props: any) {
    super(props);
    // this._target = React.createRef();
  }

  public render() {
    return (
      <View>
        <View style={{ flexDirection: "row", paddingTop: 20 }}>
          <Text style={{ padding: 10 }}>Placement Options: </Text>
          <Picker
            style={{ width: 200, height: 35 }}
            selectedValue={this.state.placementOptions}
            onValueChange={value =>
              this.setState({ placementOptions: value })
            }
          >
            {placementValues.map(item => (
              <Picker.Item key={item} label={item} value={item} />
            ))}
          </Picker>
        </View>
        <View style={{ justifyContent: "center", padding: 20, width: 200 }}>
          <Button
            onPress={this._onPress}
            title={this.state.buttonTitle}
            ref={ref => {
              // this._target = ref;
              if(!this.state.target)
                this.setState({target: ref})
            }}
          />
        </View>
        <View style={{ flexDirection: "row" }}>
          <Text style={{ padding: 10 }}>isAttachedAsContextMenu: </Text>
          <CheckBox
            style={{ justifyContent: "center", padding: 20 }}
            checked={this.state.popupCheckBoxState}
            onValueChange={value =>
              this.setState({
                popupCheckBoxState: value,
                attachAsContextMenu: value,
                isMenuFlyoutVisible: false
              })
            }
          />
        </View>
         { this.state.target && (<MenuFlyout
            isOpen={this.state.isMenuFlyoutVisible}
            attachAsContextFlyout={this.state.attachAsContextMenu}
            onDismiss={this._onMenuFlyoutDismissed}
            target={this.state.target}
            placement={this.state.placementOptions}
          >
            <MenuFlyoutItem text="Edit"></MenuFlyoutItem>
            </MenuFlyout>
            
            )}   

          <Text style={{ padding: 10 }}>{this.state.message}</Text>
 
      </View>
    );
  }


  _onPress = () => {
    if (!this.state.attachAsContextMenu){
    this.setState({ buttonTitle: 'Close MenuFlyout', isMenuFlyoutVisible: true });
    }
  }

  _onMenuFlyoutDismissed = (isOpen: boolean) => {
    this.setState({ isMenuFlyoutVisible: isOpen });
    this.setState({ buttonTitle: 'Open MenuFlyout', message: 'flyout dismissed once' });
  }
}


export const displayName = (_undefined?: string) => { };
export const title = '<MenuFlyout>';
export const description = 'Displays a Menu that can be operated like a flyout or assigned as a compnents context menu.';
export const examples = [
  {
    title: 'MenuFlyout Anchor to button',
    render: function (): JSX.Element {
      return <MenuFlyoutExample />;
    },
  }
];
