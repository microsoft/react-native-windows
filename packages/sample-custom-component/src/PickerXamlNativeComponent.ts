import {codegenNativeComponent} from 'react-native';
import type { ViewProps } from 'react-native';
import type {
  Int32,
  WithDefault,
  BubblingEventHandler,
} from 'react-native/Libraries/Types/CodegenTypes';

/**
 * Represents a single item in the PickerXaml.
 */
export type PickerXamlItem = Readonly<{
  label: string;
  value?: string;
}>;

/**
 * Event payload when the picker selection changes.
 */
export type PickerXamlChangeEvent = Readonly<{
  value: string;
  itemIndex: Int32;
  text: string;
}>;

export interface PickerXamlProps extends ViewProps {
  /**
   * Array of picker items to display.
   */
  items: ReadonlyArray<PickerXamlItem>;

  /**
   * The index of the currently selected item.
   */
  selectedIndex?: WithDefault<Int32, -1>;

  /**
   * Callback when selection changes.
   */
  onPickerSelect?: BubblingEventHandler<PickerXamlChangeEvent>;
}

export default codegenNativeComponent<PickerXamlProps>('PickerXaml');
