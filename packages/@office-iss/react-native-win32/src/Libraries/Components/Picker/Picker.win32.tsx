import * as React from 'react';
import { PickerItemProps, PickerProps, requireNativeComponent } from 'react-native';

const RCTPickerItem = requireNativeComponent('RCTPickerItem');
const RCTPicker = requireNativeComponent('RCTPicker');

class PickerItem extends React.Component<PickerItemProps, {}> {
  public render(): React.ReactNode {
    return <RCTPickerItem {...this.props} />;
  }
};

function getValueChangeEvent(pickerProps: PickerProps, children?: React.ReactNode) {
  return (event: { nativeEvent: { pickerItemIndex: number } }) => {
    const { onValueChange } = pickerProps;
    if (!onValueChange || !children) {
      return;
    }
    const pickerItems = children as PickerItem[];
    const selectedIndex = event.nativeEvent.pickerItemIndex;

    if (selectedIndex >= 0 && selectedIndex < pickerItems.length) {
      onValueChange(pickerItems[selectedIndex].props.value, selectedIndex);
    } else {
      onValueChange(null, selectedIndex);
    }
  };
}

class Picker extends React.Component<PickerProps, {}> {
  public static Item = PickerItem;

  public render(): React.ReactNode {
    const { selectedValue, children } = this.props;

    let selectedIndex = -1;
    if (selectedValue && children) {
      selectedIndex = (children as PickerItem[]).findIndex(pickerItem => {
        return pickerItem.props.value === selectedValue;
      });
    }

    const onValueChange = getValueChangeEvent(this.props, children);
    const nativeProps = {
      ...this.props,
      selectedIndex,
      onValueChange
    };

    return <RCTPicker {...nativeProps}>{children}</RCTPicker>;
  }
}

module.exports = Picker;
