/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import {ViewProps} from 'react-native';

export type Placement =
  | 'top'
  | 'bottom'
  | 'left'
  | 'right'
  | 'full'
  | 'top-edge-aligned-left'
  | 'top-edge-aligned-right'
  | 'bottom-edge-aligned-left'
  | 'bottom-edge-aligned-right'
  | 'left-edge-aligned-top'
  | 'right-edge-aligned-top'
  | 'left-edge-aligned-bottom'
  | 'right-edge-aligned-bottom';

export interface IFlyoutProps extends ViewProps {
  horizontalOffset?: number;
  isLightDismissEnabled?: boolean;
  isOpen?: boolean;
  attachAsContextFlyout?: boolean;
  onDismiss?: (isOpen: boolean) => void;
  onOpen?: () => void;
  placement?: Placement;
  target?: React.ReactNode;
  verticalOffset?: number;
}
