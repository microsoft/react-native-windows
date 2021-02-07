/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Socket} from 'net';

import {K} from 'win32-api';

const kernel32 = K.load(['GetAppContainerNamedObjectPath']);

kernel32.GetA

export class RpcClient {
  constructor(private readonly pipeConnection: Socket) {}
}

export async function connect(opts: {
  appName: string;
  pipeName: string;
}): Promise<RpcClient> {
  const;

  return new RpcClient(foo);
}
