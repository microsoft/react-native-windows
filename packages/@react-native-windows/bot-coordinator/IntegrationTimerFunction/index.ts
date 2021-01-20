/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Context} from '@azure/functions';
import {initializeActors} from '../Actors/Actor';

export default async (context: Context) => {
  const actorsHandle = await initializeActors(context);
  await actorsHandle.emit('integration-timer-fired');
};
