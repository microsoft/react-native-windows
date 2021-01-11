/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {StyleProp, ViewProps, ViewStyle} from 'react-native';

export interface GlyphStyle extends ViewStyle {
  color?: string;
}

export interface GlyphProps extends ViewProps {
  style?: StyleProp<GlyphStyle>;
  emSize?: number;
  fontUri: string;
  glyph: string;
  colorEnabled?: boolean;
}
