/**
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import * as React from 'react';
import { TextInputTestPage } from './TextInputTestPage';
import { UnknownPage } from './UnknownPage';
import {
  TEXTINPUT_TESTPAGE,
  UNKNOWN_TESTPAGE,
  LOGIN_TESTPAGE,
  ACCESSBILITY_TESTPAGE,
  DIRECT_MANIPULATION_TESTPAGE,
  IMAGE_TESTPAGE,
  CONTROL_STYLE_TESTPAGE,
  TRANSFORM_TESTPAGE,
} from './Consts';
import { LoginTestPage } from './LoginTestPage';
import { AccessibilityTestPage } from './AccessibilityTestPage';
import { DirectManipulationTestPage } from './DirectManipulationPage';
import { ImageTestPage } from './ImageTestPage';
import { ControlStyleTestPage } from './ControlStyleTestPage';
import { TransformTestPage } from './TransformTestPage';

export interface ITestPage {
  testId: string;
  description: string;
  content: React.FC | typeof React.Component;
}

const TestPages: ITestPage[] = [
  {
    testId: TEXTINPUT_TESTPAGE,
    description: 'TextInput Test Page',
    content: TextInputTestPage,
  },
  {
    testId: LOGIN_TESTPAGE,
    description: 'Login Test Page',
    content: LoginTestPage,
  },
  {
    testId: ACCESSBILITY_TESTPAGE,
    description: 'Accessiblity Test Page',
    content: AccessibilityTestPage,
  },
  {
    testId: DIRECT_MANIPULATION_TESTPAGE,
    description: 'Direct Manipulation Test Page',
    content: DirectManipulationTestPage,
  },
  {
    testId: IMAGE_TESTPAGE,
    description: 'Image Test Page',
    content: ImageTestPage,
  },
  {
    testId: CONTROL_STYLE_TESTPAGE,
    description: 'Control Style Test Page',
    content: ControlStyleTestPage,
  },
  {
    testId: TRANSFORM_TESTPAGE,
    description: 'Transform Test Page',
    content: TransformTestPage,
  },
  {
    testId: UNKNOWN_TESTPAGE,
    description: 'Unknown Page',
    content: UnknownPage,
  },
];

export default TestPages;
