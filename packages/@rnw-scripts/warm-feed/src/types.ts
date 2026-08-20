/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

export type Ecosystem = 'npm' | 'nuget';

export type WarmStatus = 'warmed' | 'missing' | 'failed';

/** A single concrete package version to warm into the feed. */
export interface WarmTarget {
  ecosystem: Ecosystem;
  id: string;
  version: string;
  /** Stage/reason that produced this target, for diagnostics. */
  source: string;
}

/** A package as the feed already knows it (from the Get Packages API). */
export interface FeedPackage {
  ecosystem: Ecosystem;
  id: string;
  /** Versions already saved in the feed. */
  savedVersions: string[];
}

export interface FeedConfig {
  npm?: {registry: string};
  nuget?: {index: string};
}

export interface EnumerateConfig {
  /**
   * Feed Management base, e.g.
   * https://feeds.dev.azure.com/<org>/<project>/_apis/packaging/Feeds/<feed>
   */
  feedManagementBase: string;
  apiVersion: string;
  pageSize: number;
}

export interface ExpandConfig {
  /**
   * 'in-use-lines' warms the latest patch of each (major,minor) already in the
   * feed; 'in-use-majors' warms the latest of each major only.
   */
  scope: 'in-use-lines' | 'in-use-majors';
  /** Also warm the newest stable version overall (imminent bumps). */
  includeLatest: boolean;
  includePrerelease: boolean;
  /** Limit to the N most-recent majors already in use (0 = no limit). */
  maxMajorsBack: number;
}

export interface WarmerConfig {
  feeds: FeedConfig;
  enumerate: EnumerateConfig;
  expand: ExpandConfig;
  concurrency: number;
  /** Entries of the form 'id', 'id@version', or 'ecosystem:id@version'. */
  ignore: string[];
}

export interface RunOptions {
  configPath: string;
  /** Raw --only value; validated to an Ecosystem in run(). */
  only?: string;
  dryRun: boolean;
  /** Warm every target, ignoring the feed's already-cached skip. */
  verify: boolean;
  concurrency?: number;
  /** Ad-hoc 'ecosystem:id@version' entries (one-off warm). */
  packages: string[];
  verbose: boolean;
}

export interface Logger {
  info(message: string): void;
  warn(message: string): void;
  error(message: string): void;
  debug(message: string): void;
}

export interface Auth {
  kind: 'pat' | 'aad';
  /** Authorization header for feed HTTP requests. */
  header(): Promise<Record<string, string>>;
}

export interface Ctx {
  config: WarmerConfig;
  options: RunOptions;
  auth: Auth;
  log: Logger;
}

export interface WarmResult {
  target: WarmTarget;
  status: WarmStatus;
  detail?: string;
}
