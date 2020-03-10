/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as ManifestData from './ManifestData';

import * as _ from 'lodash';
import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';

import GitReactFileRepository from './GitReactFileRepository';
import {OverrideFileRepository} from './FileRepository';

const DEFAULT_MERGE_DIR = path.join(os.tmpdir(), 'react-native-windows-merge');

export interface UpgradeRequest {
  overrideEntry: ManifestData.NonPlatformEntry;
  newVersion: string;
}

export interface UpgradeResult {
  overrideFile: string;
  hasConflicts: boolean;
  content: string;
}

interface MergeResult {
  hasConflicts: boolean;
  content: string;
}

export default class OverrideUpgrader {
  private reactRepo: GitReactFileRepository;
  private ovrRepo: OverrideFileRepository;
  private tmpMergeDir: string;

  constructor(
    reactRepo: GitReactFileRepository,
    ovrRepo: OverrideFileRepository,
    tmpMergeDir?: string,
  ) {
    this.reactRepo = reactRepo;
    this.ovrRepo = ovrRepo;
    this.tmpMergeDir = tmpMergeDir || DEFAULT_MERGE_DIR;
  }

  async perform(reqs: Array<UpgradeRequest>): Promise<Array<UpgradeResult>> {
    const filesToCache = _.flatMap(reqs, req => [
      {name: req.overrideEntry.baseFile, vers: req.overrideEntry.baseVersion},
      {name: req.overrideEntry.baseFile, vers: req.newVersion},
    ]);

    const sources = await ReactSourceCache.create(filesToCache, this.reactRepo);

    const results: Array<UpgradeResult> = [];

    for (const req of reqs) {
      const ovrFile = req.overrideEntry.file;
      const baseFile = req.overrideEntry.baseFile;
      const baseVersion = req.overrideEntry.baseVersion;

      const base = sources.getFileContents(baseFile, baseVersion);
      const ours = await this.ovrRepo.getFileContents(ovrFile);
      const theirs = sources.getFileContents(baseFile, req.newVersion);
      const merged = await this.threeWayMerge(base, ours, theirs);

      results.push({overrideFile: ovrFile, ...merged});
    }

    return results;
  }

  private async threeWayMerge(
    base: string,
    ours: string,
    theirs: string,
  ): Promise<MergeResult> {
    const basePath = path.join(this.tmpMergeDir, 'base');
    const oursPath = path.join(this.tmpMergeDir, 'ours');
    const theirsPath = path.join(this.tmpMergeDir, 'theirs');

    await fs.promises.writeFile(basePath, this.normLineEndings(base));
    await fs.promises.writeFile(oursPath, this.normLineEndings(ours));
    await fs.promises.writeFile(theirsPath, this.normLineEndings(theirs));

    const content = await Diff3.diff(oursPath, basePath, theirsPath);
    return {content, hasConflicts: content.includes('<<<<<<<')};
  }

  private normLineEndings(str: string): string {
    return str.replace(/([^\r])\n/g, '$1\r\n');
  }
}

/**
 * Caches requested React source files in memory. Retrieves files batched by
 * version to minimize the performance penalty of branch switching in
 * GitReactFileRepository.
 */
class ReactSourceCache {
  private cachedFiles: Array<{name: string; version: string; content: string}>;

  private constructor() {}

  static async create(
    filesToCache: Array<{name: string; vers: string}>,
    reactRepo: VersionedReactFileRepository,
  ): Promise<ReactSourceCache> {
    const cache = new ReactSourceCache();
    cache.cachedFiles = [];

    const sortedFilesToCache = filesToCache.sort((a, b) =>
      a.vers.localeCompare(b.vers),
    );

    for (const file of sortedFilesToCache) {
      const content = await reactRepo.getFileContents(file.name, file.vers);
      cache.cachedFiles.push({name: file.name, version: file.vers, content});
    }

    return cache;
  }

  getFileContents(name: string, version: string): string {
    return this.cachedFiles.find(f => f.name === name && f.version === version)
      .content;
  }
}
