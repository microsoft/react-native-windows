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
import type {CodegenTypes, ViewProps} from 'react-native';

type SelectedDatesChangedEvent = Readonly<{
  value: boolean;
  startDate: string;
}>;

export interface CalendarViewProps extends ViewProps {
  label: string;
  onSelectedDatesChanged?: CodegenTypes.DirectEventHandler<SelectedDatesChangedEvent>;
}

export default codegenNativeComponent<CalendarViewProps>('CalendarView');
