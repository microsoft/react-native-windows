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

import isUtf8 = require('isutf8');

export type HashOpts = {
  /**
   * What differences between files should we be insensitive to when generating
   * a hash? Defaults to line-ending insensitivity
   */
  insensitivity?: 'none' | 'line-ending' | 'whitespace';
};

/**
 * Normalize content according to insensitivty rules
 */
function normalizeContent(
  content: string | Buffer,
  opts: HashOpts,
): string | Buffer {
  if (
    opts.insensitivity === 'none' ||
    (typeof content !== 'string' && !isUtf8(content))
  ) {
    return content;
  }

  switch (opts.insensitivity || 'line-ending') {
    case 'line-ending':
      // Convert to CRLF for legacy hash stability
      return content.toString('utf8').replace(/(?<!\r)\n/g, '\r\n');

    case 'whitespace':
      return content
        .toString('utf8')
        .trim()
        .replace(/\s+/g, ' ');
  }
}

/**
 * Creates a hash from content, attempting to normalize for line-feeds
 */
export class Hasher {
  private readonly hash: crypto.Hash;
  private readonly hashOpts: HashOpts;

  constructor(hashOpts?: HashOpts) {
    this.hash = crypto.createHash('sha1');
    this.hashOpts = hashOpts || {};
  }

  feedContent(content: string | Buffer): Hasher {
    this.hash.update(normalizeContent(content, this.hashOpts));
    return this;
  }

  digest(): string {
    return this.hash.digest('hex');
  }
}

/**
 * Convenience helper which hashes a single buffer
 */
export function hashContent(content: string | Buffer, opts?: HashOpts): string {
  return new Hasher(opts).feedContent(content).digest();
}

/**
 * Helper to hash a file/directory belonging to a repository
 */
export async function hashFileOrDirectory(
  name: string,
  repo: FileRepository,
  opts?: HashOpts,
): Promise<string | null> {
  const type = await repo.stat(name);
  if (type === 'none') {
    return null;
  }

  if (type === 'file') {
    return hashContent((await repo.readFile(name))!, opts);
  } else {
    const hasher = new Hasher(opts);
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
