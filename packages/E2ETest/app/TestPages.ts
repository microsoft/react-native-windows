/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import * as React from 'react';
import { TextInputTestPage } from './TextInputTestPage';
import { UnknownPage } from './UnknownPage';
import { TEXTINPUT_TESTPAGE, UNKNOWN_TESTPAGE, LOGIN_TESTPAGE } from './Consts';
import { LoginTestPage } from './LoginTestPage';

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
    testId: UNKNOWN_TESTPAGE,
    description: 'Unknown Page',
    content: UnknownPage,
  },
];

export default TestPages;
