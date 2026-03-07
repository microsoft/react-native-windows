/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */
import type { ViewProps } from 'react-native';
import type { DirectEventHandler } from 'react-native/Libraries/Types/CodegenTypes';
type SelectedDatesChangedEvent = Readonly<{
    value: boolean;
    startDate: string;
}>;
export interface CalendarViewProps extends ViewProps {
    label: string;
    onSelectedDatesChanged?: DirectEventHandler<SelectedDatesChangedEvent>;
}
declare const _default: import("react-native").HostComponent<CalendarViewProps>;
export default _default;
