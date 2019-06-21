// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
import { ViewProps } from 'react-native';

export interface IThemingProps extends ViewProps {
  isHighContrast?: boolean;
  highContrastScheme?: string;
  onThemeChanged?: (event: IThemingChangedEvent) => void;
}

export interface IThemingChangedEvent {
  nativeEvent: {
      value: string;
  };
}