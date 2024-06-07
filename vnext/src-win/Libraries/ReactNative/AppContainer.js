/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

import type {RootTag} from './RootTag';
import type {ViewStyleProp} from '../StyleSheet/StyleSheet';

import * as React from 'react';

export type Props = $ReadOnly<{|
  children?: React.Node,
  fabric?: boolean,
  rootTag: number | RootTag,
  initialProps?: {...},
  showArchitectureIndicator?: boolean,
  WrapperComponent?: ?React.ComponentType<any>,
  rootViewStyle?: ?ViewStyleProp,
  internal_excludeLogBox?: boolean,
  internal_excludeInspector?: boolean,
|}>;

const AppContainer: React.AbstractComponent<Props> = __DEV__
  ? require('./AppContainer-dev').default
  : require('./AppContainer-prod').default;

module.exports = AppContainer;
