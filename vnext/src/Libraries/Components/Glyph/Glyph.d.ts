/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {Constructor} from 'Utilities';
import {NativeMethods} from '../../Renderer/shims/ReactNativeTypes';
import {ColorValue, StyleProp} from '../../StyleSheet/StyleSheet';
import {ViewStyle} from '../../StyleSheet/StyleSheetTypes';
import {ViewProps} from '../View/ViewPropTypes';

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
