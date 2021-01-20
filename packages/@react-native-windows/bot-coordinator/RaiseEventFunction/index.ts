/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {PathReporter} from 'io-ts/lib/PathReporter';
import {actorEvents, ActorEventName} from '@react-native-windows/bot-actors';
import {httpBotFunction} from '../botFunction';

export default httpBotFunction(async ({context, req, actorsHandle}) => {
  const event = parseEvent(req.body);
  if (!event) {
    context.res = {status: 400, body: 'Malformed event'};
    return;
  }

  await actorsHandle.emitEvent(event.name, event.payload);
});

/**
 * Parse a JSON body into event emitter params
 */
function parseEvent(
  bodyJson: any,
): {name: ActorEventName; payload?: any} | null {
  if (
    typeof bodyJson.event !== 'string' ||
    !actorEvents.hasOwnProperty(bodyJson.event)
  ) {
    return null;
  }

  const expectedPayload = actorEvents[bodyJson.event as ActorEventName];
  const errors = PathReporter.report(expectedPayload.decode(bodyJson.payload));
  if (errors.length > 0) {
    return null;
  }

  return {name: bodyJson.event, payload: bodyJson.payload};
}
