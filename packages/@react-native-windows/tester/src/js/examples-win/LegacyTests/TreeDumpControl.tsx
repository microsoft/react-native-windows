/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
import {requireNativeComponent, ViewStyle} from 'react-native';

type TreeDumpControlProps = {
  style: ViewStyle;
  dumpID: string;
  uiaID: string;
  additionalProperties?: string[];
};

type NativeTreeDumpControlProps = TreeDumpControlProps & {testID: string};

const NativeTreeDumpControl = requireNativeComponent<
  NativeTreeDumpControlProps
>('TreeDumpControl');

export const TreeDumpControl: React.FC<TreeDumpControlProps> = props => (
  <NativeTreeDumpControl {...props} testID="tree-dump-control" />
);
