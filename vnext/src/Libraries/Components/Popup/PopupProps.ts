// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
import { ViewProps } from 'react-native';

export interface IPopupProps extends ViewProps {
  isOpen?: boolean;
  isLightDismissEnabled?: boolean;
  horizontalOffset?: number;
  verticalOffset?: number;
  target?: React.ReactNode;
  onDismiss?: (isOpen: boolean) => void;
}