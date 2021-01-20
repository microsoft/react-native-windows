/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Context} from '@azure/functions';
import {initializeActors} from '@react-native-windows/bot-actors';
import getActorSecrets from '../getActorSecrets';

export default async (context: Context) => {
  const actorsHandle = await initializeActors(context.log, getActorSecrets());
  await actorsHandle.emitEvent('integration-timer-fired');
};
