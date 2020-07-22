/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
import { By } from '../pages/BasePage';
import { LOGIN_TESTPAGE } from 'react-native-windows/RNTester/js/examples-win/LegacyTests/Consts';

describe('SmokeTest', () => {
  it('SmokeTest', () => {
    By(LOGIN_TESTPAGE);
  });
});
