/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type { ViewProps } from 'react-native';
import type {DirectEventHandler, Int32} from 'react-native/Libraries/Types/CodegenTypes';

type SelectedDatesChangedEvent = Readonly<{
  value: boolean;
  startDate: string;
}>;

export interface CalendarViewProps extends ViewProps {
    displayMode : Int32;
    onSelectedDatesChanged?: DirectEventHandler<SelectedDatesChangedEvent>;
}

export default codegenNativeComponent<CalendarViewProps>('CalendarView');
