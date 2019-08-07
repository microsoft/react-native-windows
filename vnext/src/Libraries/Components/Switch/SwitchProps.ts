/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {ViewProps} from 'react-native';

export interface ISwitchTrackColorProps {
  false?: string;
  true?: string;
}

export interface ISwitchProps extends ViewProps {
  disabled?: boolean;
  value?: boolean;
  thumbColor?: string;
  trackColor?: ISwitchTrackColorProps;
  onChange?: (event: ISwitchChangeEvent) => void;
  onValueChange?: (value: boolean) => void;
}

export interface ISwitchChangeEvent {
  nativeEvent: {
    value: boolean;
  };
}
