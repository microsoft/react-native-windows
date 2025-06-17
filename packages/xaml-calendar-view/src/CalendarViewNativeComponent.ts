/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type { ViewProps } from 'react-native';

export interface CalendarViewProps extends ViewProps {
    label : string;
}

export default codegenNativeComponent<CalendarViewProps>('CalendarView');
