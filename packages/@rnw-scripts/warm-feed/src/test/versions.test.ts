/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  compareNuGet,
  compareSemver,
  isStable,
  isStableNuGet,
  parseNuGet,
  parseSemver,
} from '../versions';

describe('parseSemver', () => {
  test('parses a plain version', () => {
    expect(parseSemver('1.2.3')).toMatchObject({
      major: 1,
      minor: 2,
      patch: 3,
      prerelease: [],
    });
  });

  test('accepts a leading v and build metadata, dropping the build', () => {
    expect(parseSemver('v1.2.3+build.5')).toMatchObject({
      major: 1,
      minor: 2,
      patch: 3,
      prerelease: [],
    });
  });

  test('splits dotted prerelease identifiers', () => {
    expect(parseSemver('1.0.0-alpha.1')?.prerelease).toEqual(['alpha', '1']);
  });

  test('rejects partial and non-numeric versions', () => {
    expect(parseSemver('1.2')).toBeNull();
    expect(parseSemver('1.2.x')).toBeNull();
    expect(parseSemver('not-a-version')).toBeNull();
  });
});

describe('compareSemver', () => {
  const parse = (v: string) => {
    const p = parseSemver(v);
    if (!p) throw new Error(`bad test version ${v}`);
    return p;
  };
  const cmp = (a: string, b: string) =>
    Math.sign(compareSemver(parse(a), parse(b)));

  test('orders by major, minor, then patch', () => {
    expect(cmp('2.0.0', '1.9.9')).toBe(1);
    expect(cmp('1.2.0', '1.1.9')).toBe(1);
    expect(cmp('1.1.2', '1.1.1')).toBe(1);
    expect(cmp('1.1.1', '1.1.1')).toBe(0);
  });

  test('a prerelease sorts below its release', () => {
    expect(cmp('1.0.0-rc.1', '1.0.0')).toBe(-1);
  });

  test('numeric prerelease identifiers sort below alphanumeric ones', () => {
    expect(cmp('1.0.0-1', '1.0.0-alpha')).toBe(-1);
  });

  test('more prerelease fields win when the shared ones are equal', () => {
    expect(cmp('1.0.0-alpha.1', '1.0.0-alpha')).toBe(1);
  });

  test('isStable is false only for prereleases', () => {
    expect(isStable(parse('1.0.0'))).toBe(true);
    expect(isStable(parse('1.0.0-beta'))).toBe(false);
  });
});

describe('parseNuGet', () => {
  test('pads to four parts', () => {
    expect(parseNuGet('1.2')?.parts).toEqual([1, 2, 0, 0]);
    expect(parseNuGet('1.2.3.4')?.parts).toEqual([1, 2, 3, 4]);
  });

  test('lower-cases prerelease identifiers', () => {
    expect(parseNuGet('1.0.0-Beta')?.prerelease).toEqual(['beta']);
  });

  test('rejects more than four parts and empty input', () => {
    expect(parseNuGet('1.2.3.4.5')).toBeNull();
    expect(parseNuGet('')).toBeNull();
  });
});

describe('compareNuGet', () => {
  const parse = (v: string) => {
    const p = parseNuGet(v);
    if (!p) throw new Error(`bad test version ${v}`);
    return p;
  };
  const cmp = (a: string, b: string) =>
    Math.sign(compareNuGet(parse(a), parse(b)));

  test('treats a missing trailing part as zero', () => {
    expect(cmp('1.2', '1.2.0.0')).toBe(0);
    expect(cmp('1.2.1', '1.2')).toBe(1);
  });

  test('a prerelease sorts below its release', () => {
    expect(cmp('1.0.0-preview', '1.0.0')).toBe(-1);
  });

  test('isStableNuGet is false only for prereleases', () => {
    expect(isStableNuGet(parse('2.0.0'))).toBe(true);
    expect(isStableNuGet(parse('2.0.0-preview.1'))).toBe(false);
  });
});
