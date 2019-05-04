// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
import { ViewProps } from 'react-native';

export interface IFlyoutProps extends ViewProps {
  isLightDismissEnabled?: boolean;
  isOpen?: boolean;
  onDismiss?: (isOpen: boolean) => void;
  placement?: 'top' | 'bottom' | 'left' | 'right' | 'full';
  target?: React.ReactNode;
}