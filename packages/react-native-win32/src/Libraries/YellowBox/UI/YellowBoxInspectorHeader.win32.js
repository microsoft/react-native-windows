/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @flow
 * @format
 */

'use strict';
const React = require('react');
const View = require('../../Components/View/View');

import type YellowBoxWarning from '../Data/YellowBoxWarning';

type Props = $ReadOnly<{|
  onSelectIndex: (selectedIndex: number) => void,
  selectedIndex: number,
  warnings: $ReadOnlyArray<YellowBoxWarning>,
|}>;

// The RN YellowBoxInspectorHeader uses PixelRatio, which isn't multi DPI aware.
// So we just stub the header for now
const YellowBoxInspectorHeader = (props: Props): React.Node => <View />;
module.exports = YellowBoxInspectorHeader;
