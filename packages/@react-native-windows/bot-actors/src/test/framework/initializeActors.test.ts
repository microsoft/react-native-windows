/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {initializeActors, failOnErrorLogger, fakeSecrets} from 'bot-actors';

test('Initialization succeeds', async () => {
  await initializeActors(failOnErrorLogger, fakeSecrets);
});
