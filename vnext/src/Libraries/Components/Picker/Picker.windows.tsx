/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {requireNativeComponent, processColor} from 'react-native';
import {
  IPickerItemProps,
  IPickerProps,
  IPickerChangeEvent,
} from './PickerProps';

const RCTPicker = requireNativeComponent('RCTPicker');

class PickerItem extends React.Component<IPickerItemProps> {
  public render(): JSX.Element | null {
    return null;
  }
}

export interface IPickerItemData {
  label: string;
  // tslint:disable-next-line:no-any
  value?: any;
  textColor?: number;
}

// tslint:disable-next-line:interface-name
interface State {
  selectedIndex: number;
  items: IPickerItemData[];
}

type PickerPropsWithChildren = Readonly<{children?: React.ReactNode}> &
  Readonly<IPickerProps>;

/**
 * Picker is a controlled component, which expects the selectedValue prop to be updated
 * whenever selection changes, or selection will revert to the prop selectedValue
 *
 * when using editable=true, onValueChange can be called with a selectedValue of null &
 * Index of -1, and text will be provided.
 * To maintain the text in the controlled component, props should reflect
 * that state by specifying selectedValue of null and specify the text property.
 */
export class Picker extends React.Component<IPickerProps, State> {
  public static Item = PickerItem;

  // tslint:disable-next-line:no-any
  private _rctPicker: any;

  public static getDerivedStateFromProps(
    props: PickerPropsWithChildren,
  ): State {
    let selectedIndex = -1;
    const items: IPickerItemData[] = [];
    React.Children.toArray(props.children).forEach(
      (c: React.ReactNode, index: number) => {
        const child = (c as unknown) as PickerItem;
        if (child.props.value === props.selectedValue) {
          selectedIndex = index;
        }
        items.push({
          value: child.props.value,
          label: child.props.label,
          textColor: processColor(child.props.color),
        });
      },
    );
    return {selectedIndex, items};
  }

  public constructor(props: IPickerProps) {
    super(props);
    this._rctPicker = React.createRef();
    this.state = {
      selectedIndex: 0,
      items: [],
    };
  }

  public render(): JSX.Element {
    const props = {...this.props};
    props.onStartShouldSetResponder = () => true;
    props.onResponderTerminationRequest = () => false;
    props.style = this.props.style;

    return (
      <RCTPicker
        {...props}
        items={this.state.items}
        selectedIndex={this.state.selectedIndex}
        onChange={this._onChange}
        ref={this._setRef}>
        {this.props.children}
      </RCTPicker>
    );
  }

  private _setRef = (comboBox: Picker /*RCTPicker*/) => {
    this._rctPicker = comboBox;
  };

  private _onChange = (event: IPickerChangeEvent) => {
    if (this._rctPicker) {
      this._rctPicker.setNativeProps({
        selectedIndex: this.state.selectedIndex,
        text: this.props.text,
      });
    }

    this.props.onChange && this.props.onChange(event);
    this.props.onValueChange &&
      this.props.onValueChange(
        event.nativeEvent.value,
        event.nativeEvent.itemIndex,
        event.nativeEvent.text,
      );
  };
}

export default Picker;
