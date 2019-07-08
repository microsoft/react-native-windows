// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
import { ViewProps } from 'react-native';

export interface IMenuFlyoutItemProps extends ViewProps {

  onClick?: () => void;
  text?: string;
}
