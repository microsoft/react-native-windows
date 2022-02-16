/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
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

export type ShowMode =
  | 'auto'
  | 'standard'
  | 'transient'
  | 'transient-with-dismiss-on-pointer-move-away';

export interface IFlyoutProps extends ViewProps {
  horizontalOffset?: number;
  isLightDismissEnabled?: boolean;
  autoFocus?: boolean;
  shouldConstrainToRootBounds?: boolean;

  /**
   * Specifies whether the area outside the flyout is darkened
   */
  isOverlayEnabled?: boolean;

  isOpen?: boolean;
  onDismiss?: (isOpen: boolean) => void;
  placement?: Placement;
  showMode?: ShowMode;
  target?: React.ReactNode;
  verticalOffset?: number;
}
