/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {actorFunction} from '../actorFunction';

export default actorFunction(async ({actorsHandle}) => {
  await actorsHandle.emitEvent('integration-timer-fired');
});
