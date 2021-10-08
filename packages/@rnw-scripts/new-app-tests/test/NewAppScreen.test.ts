/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';

test('Verify presence of NewAppScreen Header', async () => {
  const header = await app.findElementByTestID('new-app-screen-header');
  await header.waitForDisplayed();
});
