/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as crypto from 'crypto';
import * as path from 'path';
import {normalizePath, unixPath} from './PathUtils';
import FileRepository from './FileRepository';
import isutf8 from 'isutf8';

/**
 * Creates a hash from content, attempting to normalize for line-feeds
 */
export class Hasher {
  private hash: crypto.Hash;

  constructor() {
    this.hash = crypto.createHash('sha1');
  }

  feedContent(content: string | Buffer): Hasher {
    if (typeof content === 'string' || isutf8(content)) {
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
  return new Hasher().feedContent(content).digest();
}

/**
 * Helper to hash a file/directory belonging to a repository
 */
export async function hashFileOrDirectory(
  name: string,
  repo: FileRepository,
): Promise<string | null> {
  const type = await repo.stat(name);
  if (type === 'none') {
    return null;
  }

  if (type === 'file') {
    return hashContent((await repo.readFile(name))!);
  } else {
    const hasher = new Hasher();
    const subfiles = await repo.listFiles([`${unixPath(name)}/**`]);

    for (const file of subfiles.sort()) {
      const contents = await repo.readFile(file);
      hasher.feedContent(contents!);

      // Incorporate the filename to detect if renames happen
      const platformIndependentPath = unixPath(
        path.relative(name, normalizePath(file)),
      );
      hasher.feedContent(platformIndependentPath);
    }
    return hasher.digest();
  }
}
