/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Minimal version parsing/comparison for npm and NuGet. We only need parse,
 * compare, and stability checks (for "latest patch per line"), so we avoid the
 * `semver` dependency.
 *
 * @format
 */

// --- npm semver --------------------------------------------------------------

export interface SemVer {
  major: number;
  minor: number;
  patch: number;
  prerelease: string[];
  raw: string;
}

const SEMVER_RE =
  /^v?(\d+)\.(\d+)\.(\d+)(?:-([0-9A-Za-z-.]+))?(?:\+[0-9A-Za-z-.]+)?$/;

export function parseSemver(v: string): SemVer | null {
  const m = SEMVER_RE.exec(v.trim());
  if (!m) return null;
  return {
    major: Number(m[1]),
    minor: Number(m[2]),
    patch: Number(m[3]),
    prerelease: m[4] ? m[4].split('.') : [],
    raw: v,
  };
}

export function isStable(v: SemVer): boolean {
  return v.prerelease.length === 0;
}

function comparePrerelease(a: string[], b: string[]): number {
  if (a.length === 0 && b.length === 0) return 0;
  if (a.length === 0) return 1;
  if (b.length === 0) return -1;
  const n = Math.min(a.length, b.length);
  for (let i = 0; i < n; i++) {
    const x = a[i];
    const y = b[i];
    const xn = /^\d+$/.test(x);
    const yn = /^\d+$/.test(y);
    if (xn && yn) {
      const d = Number(x) - Number(y);
      if (d !== 0) return d;
    } else if (xn) {
      return -1;
    } else if (yn) {
      return 1;
    } else if (x !== y) {
      return x < y ? -1 : 1;
    }
  }
  // A larger set of prerelease fields has higher precedence when all shared
  // identifiers are equal (semver spec).
  return a.length - b.length;
}

export function compareSemver(a: SemVer, b: SemVer): number {
  if (a.major !== b.major) return a.major - b.major;
  if (a.minor !== b.minor) return a.minor - b.minor;
  if (a.patch !== b.patch) return a.patch - b.patch;
  return comparePrerelease(a.prerelease, b.prerelease);
}

// --- NuGet -------------------------------------------------------------------

export interface NuGetVersion {
  parts: number[]; // always length 4
  prerelease: string[];
  raw: string;
}

const NUGET_RE =
  /^(\d+(?:\.\d+){0,3})(?:-([0-9A-Za-z-.]+))?(?:\+[0-9A-Za-z-.]+)?$/;

export function parseNuGet(v: string): NuGetVersion | null {
  const s = v.trim();
  const m = NUGET_RE.exec(s);
  if (!m) return null;
  const parts = m[1].split('.').map(Number);
  while (parts.length < 4) parts.push(0);
  return {parts, prerelease: m[2] ? m[2].toLowerCase().split('.') : [], raw: s};
}

export function isStableNuGet(v: NuGetVersion): boolean {
  return v.prerelease.length === 0;
}

export function compareNuGet(a: NuGetVersion, b: NuGetVersion): number {
  for (let i = 0; i < 4; i++) {
    if (a.parts[i] !== b.parts[i]) return a.parts[i] - b.parts[i];
  }
  return comparePrerelease(a.prerelease, b.prerelease);
}
