import * as React from 'react';
import { IRNTesterPage } from 'src/RNTester/RNTester.types';
import { Image } from 'react-native';

const testImage = require('./img/dpitest.png');

const ImageWin32TestPage: IRNTesterPage = {
  title: '<ImageWin32Test>',
  displayName: 'Image Win32 test',
  description: 'Image Win32 test',
  examples: [
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
      }
    }
  ]
};

export = ImageWin32TestPage;