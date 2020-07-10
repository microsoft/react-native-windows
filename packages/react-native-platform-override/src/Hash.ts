/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as crypto from 'crypto';
import isutf8 from 'isutf8';

/**
 * Creates a hash from content, attempting to normalize for line-feeds
 */
export class Hasher {
  private hash: crypto.Hash;

  constructor() {
    this.hash = crypto.createHash('sha1');
  }

  feedContent(content: Buffer): Hasher {
    if (isutf8(content)) {
      // Content is valid UTF8. Normalize line endings
      const normalizedStr = content
        .toString('utf8')
        .replace(/(?<!\r)\n/g, '\r\n');

      this.hash.update(normalizedStr);
    } else {
      // Content is binary. Hash the raw bytes
      this.hash.update(content);
    }

    return this;
  }

  digest(): string {
    return this.hash.digest('hex');
  }
}

/**
 * Convenience helper which hashes a single buffer
 */
export function hashContent(content: string | Buffer): string {
  if (typeof content === 'string') {
    content = Buffer.from(content);
  }

  return new Hasher().feedContent(content).digest();
}
