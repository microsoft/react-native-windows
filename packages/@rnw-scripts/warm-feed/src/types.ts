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
  closure: ClosureConfig;
}

/** Config for the closure/graph warming feature (see closure.ts). */
export interface ClosureConfig {
  /** npm registry to resolve against; defaults to feeds.npm.registry. */
  registry?: string;
  /** Per-module config blocks, keyed by registered special-module name. */
  modules: Record<string, ClosureModuleConfig>;
}

export interface ClosureModuleConfig {
  /** Run this module on the scheduled pass. Defaults to true when the block exists. */
  enabled?: boolean;
  /** Module-specific fields (validated by each module). */
  [key: string]: unknown;
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
  /** Ad-hoc closure roots: 'npm:id@version' whose full graph is warmed. */
  closureRoots: string[];
  /** package.json paths whose external-dependency closure is warmed. */
  closureManifests: string[];
  /** Registered special-module names to run (empty => enabled config modules). */
  closureModules: string[];
  /** Repo root for modules that read repo files (defaults to process.cwd()). */
  repoRoot?: string;
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
  /**
   * The raw feed credential (PAT or AAD access token), for an npm `.npmrc`
   * `:_authToken=` line. Azure Artifacts accepts either token form there.
   */
  token(): Promise<string>;
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
