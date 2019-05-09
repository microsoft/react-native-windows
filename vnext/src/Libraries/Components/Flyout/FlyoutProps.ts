// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
import { ViewProps } from 'react-native';

export enum Placement {
  top = 'top',
  bottom = 'bottom',
  left = 'left',
  right = 'right',
  full = 'full',
  topLeft = 'top-edge-aligned-left',
  topRight = 'top-edge-aligned-right',
  bottomLeft = 'bottom-edge-aligned-left',
  bottomRight = 'bottom-edge-aligned-right',
  leftTop = 'left-edge-aligned-top',
  rightTop = 'right-edge-aligned-top',
  leftBottom = 'left-edge-aligned-bottom',
  rightBottom = 'right-edge-aligned-bottom'
}

export interface IFlyoutProps extends ViewProps {
  isLightDismissEnabled?: boolean;
  isOpen?: boolean;
  onDismiss?: (isOpen: boolean) => void;
  placement?: Placement;
  target?: React.ReactNode;
}