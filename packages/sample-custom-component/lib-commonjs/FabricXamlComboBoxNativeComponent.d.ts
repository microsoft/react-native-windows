/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */
import type { ViewProps } from 'react-native';
import type { DirectEventHandler, Int32 } from 'react-native/Libraries/Types/CodegenTypes';
type SelectionChangedEvent = Readonly<{
    selectedIndex: Int32;
    selectedValue: string;
}>;
export interface ComboBoxProps extends ViewProps {
    selectedIndex?: Int32;
    placeholder?: string;
    onSelectionChanged?: DirectEventHandler<SelectionChangedEvent>;
}
declare const _default: import("react-native").HostComponent<ComboBoxProps>;
export default _default;
