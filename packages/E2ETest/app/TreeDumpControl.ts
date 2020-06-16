/**
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { requireNativeComponent, ViewStyle } from 'react-native';
export const TreeDumpControl = requireNativeComponent<{
  style: ViewStyle;
  dumpID: string;
  uiaID: string;
  testID: string;
  additionalProperties?: string[];
}>('TreeDumpControl');
