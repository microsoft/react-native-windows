/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {ViewProps} from 'react-native';

export interface IPopupProps extends ViewProps {
  isOpen?: boolean;

  /**
   * Configures the Popup with a transparent backdrop.
   */
  isLightDismissEnabled?: boolean;

  /**
   * Sets whether the Popup will automatically receive focus when opened.
   * Defaults to true. Always true when isLightDismissEnabled === true.
   */
  autoFocus?: boolean;

  horizontalOffset?: number;
  verticalOffset?: number;

  /**
   * Sets a React element to use as an anchor point. When set, the popup will be positioned relative to it.
   */
  target?: React.ReactNode;
  onDismiss?: () => void;
}
