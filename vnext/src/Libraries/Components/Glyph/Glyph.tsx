/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {requireNativeComponent} from 'react-native';
import {GlyphProps} from './GlyphProps';

const RCTGlyph = requireNativeComponent('PLYIcon');

/**
 * Glyph is a wrapper for the Xaml Glyph control
 *
 * This control is used to render Glyphs from a Font file, which might be
 * used similar to SVG to have vector based images that also have the
 * ability to be rendered with a specific color
 */
export class Glyph extends React.PureComponent<GlyphProps> {
  public render(): JSX.Element {
    const props = {...this.props};
    props.style = this.props.style;

    return <RCTGlyph {...props} />;
  }
}
