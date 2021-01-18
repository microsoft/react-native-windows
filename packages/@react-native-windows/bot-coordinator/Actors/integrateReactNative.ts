/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {ActorRegistry} from './Actor';

/**
 * Actor to trigger automated integration of newly published react-native builds
 */
export default ActorRegistry.register('integrateReactNative', async context => {
  context.actorEvents.on('integration-timer-fired', async () => {
    // Not yet implemented
  });
});
