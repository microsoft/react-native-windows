/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {StyleProp, ViewProps, ViewStyle} from 'react-native';

// tslint:disable-next-line:interface-name
export interface GlyphStyle extends ViewStyle {
  color?: string;
}

// tslint:disable-next-line:interface-name
export interface GlyphProps extends ViewProps {
  style?: StyleProp<GlyphStyle>;
  emSize?: number;
  fontUri: string;
  glyph: string;
  colorEnabled?: boolean;
}
