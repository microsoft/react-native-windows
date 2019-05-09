// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
import { ViewProps } from 'react-native';

export enum PlacementEnum {
  top = 'top',
  bottom = 'bottom',
  left = 'left',
  right = 'right',
  full = 'full',
  topLeft = 'top-left',
  topRight = 'top-right',
  bottomLeft = 'bottom-left',
  bottomRight = 'bottom-right',
  leftTop = 'left-top',
  rightTop = 'right-top',
  leftBottom = 'left-bottom',
  rightBottom = 'right-bottom'
}

export interface IFlyoutProps extends ViewProps {
  isLightDismissEnabled?: boolean;
  isOpen?: boolean;
  onDismiss?: (isOpen: boolean) => void;
  placement?: PlacementEnum;
  target?: React.ReactNode;
}