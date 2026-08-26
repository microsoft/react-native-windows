/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Special-module contract. A special module knows how to reproduce a specific
 * install closure that the feed-centric enumeration can't discover on its own
 * (e.g. a scaffolded project). Each module turns its config into one or more
 * dependency-spec sets; the closure engine resolves and warms them.
 *
 * @format
 */

import type {NpmRegistry} from '../registries';
import type {DepSpecSet} from '../closure';
import type {Ctx} from '../types';

export interface SpecialModuleContext {
  ctx: Ctx;
  /** RNW repo root, for modules that read repo files (e.g. vnext/package.json). */
  repoRoot: string;
  /** npm registry client (feed), for resolving version lines. */
  registry: NpmRegistry;
  /** npm registry URL (feed), for npm/npx subprocesses. */
  npmRegistryUrl: string;
  /** Authenticated `.npmrc` path for npm/npx subprocesses. */
  npmrcPath: string;
}

export interface SpecialModule {
  /** Stable name used in config (`closure.modules.<name>`) and `--closure-module`. */
  name: string;
  /** Produce the dependency-spec sets to resolve and warm. */
  collectDepSpecs(
    mctx: SpecialModuleContext,
    config: Record<string, unknown>,
  ): Promise<DepSpecSet[]>;
}
