/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type * as React from 'react';
import type {Constructor} from '../../../types/private/Utilities';
import type {NativeMethods} from '../../../types/public/ReactNativeTypes';
import type {ColorValue, StyleProp} from '../../StyleSheet/StyleSheet';
import type {ViewStyle} from '../../StyleSheet/StyleSheetTypes';
import type {ViewProps} from '../View/ViewPropTypes';

export interface GlyphStyle extends ViewStyle {
  color?: ColorValue | undefined;
}

export interface GlyphProps extends ViewProps {
  style?: StyleProp<GlyphStyle>;
  emSize?: number;
  fontUri: string;
  glyph: string;
  colorEnabled?: boolean;
}

declare class GlyphComponent extends React.Component<GlyphProps> {}
declare const GlyphBase: Constructor<NativeMethods> & typeof GlyphComponent;

export class Glyph extends GlyphBase {}
