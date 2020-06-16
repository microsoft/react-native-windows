/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as crypto from 'crypto';

/**
 * Hash content into the form expected in a manifest entry.
 */
export function hashContent(str: string) {
  const hasher = crypto.createHash('sha1');
  const normalizedStr = str.replace(/(?<!\r)\n/g, '\r\n');
  hasher.update(normalizedStr);
  return hasher.digest('hex');
}
