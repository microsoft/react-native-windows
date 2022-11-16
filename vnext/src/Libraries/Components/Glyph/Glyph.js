/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @flow strict-local
 * @format
 */

import * as React from 'react';

import GlyphNativeComponent from './GlyphNativeComponent';

import type {ViewProps} from '../View/ViewPropTypes';
import useMergeRefs from '../../Utilities/useMergeRefs';

export type Props = $ReadOnly<{|
  ...ViewProps,

  emSize?: number,
  fontUri: string,
  glyph: string,
  colorEnabled?: boolean,
|}>;

/**
 * Glyph is a wrapper for the Xaml Glyph control
 *
 * This control is used to render Glyphs from a Font file, which might be
 * used similar to SVG to have vector based images that also have the
 * ability to be rendered with a specific color
 */

export const Glyph: React.AbstractComponent<
  Props,
  React.ElementRef<typeof GlyphNativeComponent>,
> = React.forwardRef(function Glyph(props, forwardedRef): React.Node {
  const nativeSwitchRef = React.useRef<React.ElementRef<
    typeof GlyphNativeComponent,
  > | null>(null);

  const ref = useMergeRefs(nativeSwitchRef, forwardedRef);

  return <GlyphNativeComponent {...props} ref={ref} />;
});
