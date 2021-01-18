/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Context, HttpRequest} from '@azure/functions';

export default async (context: Context, req: HttpRequest) => {
  context.log('Hello world');
};
