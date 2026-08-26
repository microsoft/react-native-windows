/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {groupRootSpecs, parseSpec} from '../run';

describe('parseSpec (one-off --packages CLI input)', () => {
  test('parses an npm spec', () => {
    const warn = jest.fn();
    expect(parseSpec('npm:left-pad@1.3.0', warn)).toEqual({
      ecosystem: 'npm',
      id: 'left-pad',
      version: '1.3.0',
      source: 'cli-packages',
    });
    expect(warn).not.toHaveBeenCalled();
  });

  test('parses a NuGet spec case-insensitively', () => {
    const warn = jest.fn();
    expect(parseSpec('NuGet:Newtonsoft.Json@13.0.3', warn)).toMatchObject({
      ecosystem: 'nuget',
      id: 'Newtonsoft.Json',
      version: '13.0.3',
    });
  });

  test('keeps the scope on a scoped npm package', () => {
    const warn = jest.fn();
    expect(parseSpec('npm:@react-native/community@1.0.0', warn)).toMatchObject({
      id: '@react-native/community',
      version: '1.0.0',
    });
  });

  test('warns and returns null without an ecosystem prefix', () => {
    const warn = jest.fn();
    expect(parseSpec('left-pad@1.3.0', warn)).toBeNull();
    expect(warn).toHaveBeenCalledTimes(1);
  });

  test('warns and returns null when @version is missing', () => {
    const warn = jest.fn();
    expect(parseSpec('npm:left-pad', warn)).toBeNull();
    expect(warn).toHaveBeenCalledTimes(1);
  });

  test('ignores blank entries without warning', () => {
    const warn = jest.fn();
    expect(parseSpec('   ', warn)).toBeNull();
    expect(warn).not.toHaveBeenCalled();
  });
});

describe('groupRootSpecs (--closure duplicate handling)', () => {
  test('packs distinct names into a single layer', () => {
    expect(
      groupRootSpecs([
        {id: 'foo', version: '1.0.0'},
        {id: 'bar', version: '2.0.0'},
      ]),
    ).toEqual([{foo: '1.0.0', bar: '2.0.0'}]);
  });

  test('splits a repeated name with different versions into separate layers', () => {
    expect(
      groupRootSpecs([
        {id: 'foo', version: '1.0.0'},
        {id: 'foo', version: '2.0.0'},
        {id: 'bar', version: '3.0.0'},
      ]),
    ).toEqual([{foo: '1.0.0', bar: '3.0.0'}, {foo: '2.0.0'}]);
  });

  test('an empty list yields no layers', () => {
    expect(groupRootSpecs([])).toEqual([]);
  });
});
