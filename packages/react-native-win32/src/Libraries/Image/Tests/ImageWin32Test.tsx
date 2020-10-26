import * as React from 'react';
import { Image } from 'react-native';

const testImage = require('./img/dpitest.png');

export const title = 'ImageWin32Test';
export const displayName = 'Image Win32 test';
export const description = 'Image Win32 test';
export const examples = [
    {
      title: 'Win32 Image control test',
      description: 'Test Image',
      render(): JSX.Element {
        return (
          <Image
            style={ { width: 100, height: 100 } }
            source={ testImage }
          />
        );
      },
    },
  ];
