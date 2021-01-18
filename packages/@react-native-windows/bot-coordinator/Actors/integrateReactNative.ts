/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Actor, ActorRegistry, ActorContext} from './Actor';

/**
 * Actor to trigger automated integration of newly published react-native builds
 */
const integrateReactNative: Actor = async (context: ActorContext) => {
  context.actorEvents.on('integration-timer-fired', () => {
    // Not yet implemented
  });
};

export default integrateReactNative;
ActorRegistry.register('integrateReactNative', integrateReactNative);
