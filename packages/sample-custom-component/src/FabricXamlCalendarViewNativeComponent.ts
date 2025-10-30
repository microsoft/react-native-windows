/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

// Temporary test example for UseExperimentalWinUI3=true
// Remove when we get react-native-xaml working well for Fabric

import {codegenNativeComponent} from 'react-native';
import type {ViewProps} from 'react-native';
import type {DirectEventHandler} from 'react-native/Libraries/Types/CodegenTypes';

type SelectedDatesChangedEvent = Readonly<{
  value: boolean;
  startDate: string;
}>;

export interface CalendarViewProps extends ViewProps {
  label: string;
  onSelectedDatesChanged?: DirectEventHandler<SelectedDatesChangedEvent>;
}

export default codegenNativeComponent<CalendarViewProps>('CalendarView');
