/**
 * @providesModule Image
 */

'use strict';

import * as React from 'react';
import { requireNativeComponent } from 'react-native';
import { IImageProps, IImageState } from './ImageTypes';
const NativeModules = require('NativeModules');
const resolveAssetSource = require('resolveAssetSource');

const ImageLoader = NativeModules.ImageLoader;

const RCTImage: React.ComponentClass<IImageProps> = requireNativeComponent('RCTImage');

class Image extends React.Component<IImageProps, IImageState> {
  public static getSize(
    uri: string,
    success: (width: number, height: number) => void,
    // tslint:disable-next-line no-any
    failure?: (error: any) => void
  ) {
    ImageLoader.getSize(uri, (width: number, height: number, err?: string) => {
      if (!err) {
        success(width, height);
      } else {
        if (failure) {
          failure(err);
        } else {
          console.warn('Failure to get size for image: ' + uri);
        }
      }
    });
  }

  public render() {
    const props = { ...this.props };
    if (this.props.source) {
      props.source = typeof this.props.source === 'string' ? { uri: this.props.source } : resolveAssetSource(this.props.source);
    }
    return <RCTImage {...props} />;
  }
}

export = Image;
