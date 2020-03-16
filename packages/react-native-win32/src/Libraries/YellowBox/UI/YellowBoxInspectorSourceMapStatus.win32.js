/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

const React = require('react');
const Text = require('../../Text/Text');
import type {PressEvent} from '../../Types/CoreEventTypes';

// [Win32] - Replace implementation since react-native uses Animated.Image which win32 doesn't support

type Props = $ReadOnly<{|
  onPress?: ?(event: PressEvent) => void,
  status: 'COMPLETE' | 'FAILED' | 'NONE' | 'PENDING',
|}>;

const YellowBoxInspectorSourceMapStaus = (props: Props): React.Node => {
  return <Text>{props.status}</Text>;
};
module.exports = YellowBoxInspectorSourceMapStaus;
