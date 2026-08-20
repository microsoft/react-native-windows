/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {Ctx, Ecosystem, WarmResult, WarmTarget} from './types';
import type {NpmRegistry, NuGetRegistry} from './registries';
import {warmGet} from './http';

export interface Warmer {
  warm(target: WarmTarget): Promise<WarmResult>;
}

interface Registries {
  npm?: NpmRegistry;
  nuget?: NuGetRegistry;
}

/**
 * Warm an npm version by fetching its tarball through the authenticated feed,
 * which triggers the upstream save. Idempotent.
 */
function createNpmWarmer(ctx: Ctx, npm: NpmRegistry): Warmer {
  return {
    async warm(target: WarmTarget): Promise<WarmResult> {
      const tarball = await npm.getTarballUrl(target.id, target.version);
      if (!tarball) {
        return {target, status: 'missing', detail: 'version not in packument'};
      }
      const headers = await ctx.auth.header();
      const status = await warmGet(tarball, headers);
      if (status === 401 || status === 403) {
        throw new Error(
          `npm auth failed (${status}) fetching ${target.id} tarball`,
        );
      }
      if (status >= 200 && status < 300) return {target, status: 'warmed'};
      if (status === 404)
        return {target, status: 'missing', detail: 'tarball 404'};
      return {target, status: 'failed', detail: `HTTP ${status}`};
    },
  };
}

/**
 * Warm a NuGet version by fetching its `.nupkg` from the feed's flat2 endpoint,
 * which triggers the upstream save. Idempotent.
 */
function createNuGetWarmer(ctx: Ctx, nuget: NuGetRegistry): Warmer {
  return {
    async warm(target: WarmTarget): Promise<WarmResult> {
      const url = await nuget.nupkgUrl(target.id, target.version);
      const headers = await ctx.auth.header();
      const status = await warmGet(url, headers);
      if (status === 401 || status === 403) {
        throw new Error(
          `nuget auth failed (${status}) fetching ${target.id}.${target.version}`,
        );
      }
      if (status >= 200 && status < 300) return {target, status: 'warmed'};
      if (status === 404)
        return {target, status: 'missing', detail: 'nupkg 404'};
      return {target, status: 'failed', detail: `HTTP ${status}`};
    },
  };
}

export function createWarmers(
  ctx: Ctx,
  registries: Registries,
): Partial<Record<Ecosystem, Warmer>> {
  const warmers: Partial<Record<Ecosystem, Warmer>> = {};
  if (registries.npm) warmers.npm = createNpmWarmer(ctx, registries.npm);
  if (registries.nuget)
    warmers.nuget = createNuGetWarmer(ctx, registries.nuget);
  return warmers;
}
