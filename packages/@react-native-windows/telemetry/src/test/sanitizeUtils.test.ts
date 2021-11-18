/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import path from 'path';

import * as sanitizeUtils from '../utils/sanitizeUtils';

const projectDir = process.cwd();

test('getAnonymizedPath() with project dir is anonymized', () => {
  const originalPath = projectDir;
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath.startsWith('[project_dir]')).toBe(true);
});

test('getAnonymizedPath() with project/index.js is anonymized', () => {
  const originalPath = path.join(projectDir, 'index.js');
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath.startsWith('[project_dir]\\???.js')).toBe(true);
});

test('getAnonymizedPath() with project/windows/test.sln is anonymized', () => {
  const originalPath = path.normalize(
    path.join(projectDir, 'windows/test.sln'),
  );
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath.startsWith('[windows]\\???.sln')).toBe(true);
});

test('getAnonymizedPath() with project/node_modules for untracked package is anonymized', () => {
  const originalPath = path.normalize(
    path.join(projectDir, 'node_modules/untracked/index.js'),
  );
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath.startsWith('[node_modules]\\???.js')).toBe(true);
});

test('getAnonymizedPath() with project/node_modules for react-secret-pii package is anonymized', () => {
  const originalPath = path.normalize(
    path.join(projectDir, 'node_modules/react-secret-pii/index.js'),
  );
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath.startsWith('[node_modules]\\???.js')).toBe(true);
});

test('getAnonymizedPath() with path under react-native-windows is anonymized', () => {
  const originalPath = path.normalize(
    path.join(projectDir, 'node_modules/react-native-windows/index.js'),
  );
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath).toBe('[node_modules]\\react-native-windows\\index.js');
});

test('getAnonymizedPath() with path under @react-native-windows/cli is anonymized', () => {
  const originalPath = path.normalize(
    path.join(projectDir, 'node_modules/@react-native-windows/cli/index.js'),
  );
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath).toBe(
    '[node_modules]\\@react-native-windows\\cli\\index.js',
  );
});

test('getAnonymizedPath() with path under %%LocalAppData%% is anonymized', () => {
  const originalPath = path.normalize(
    path.join(process.env.LocalAppData!, 'test.sln'),
  );
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath.startsWith('[LocalAppData]\\???')).toBe(true);
});

test('getAnonymizedPath() with arbitrary path not under project dir is anonymized', () => {
  const originalPath = 'test.sln';
  const anonymizedPath = sanitizeUtils.getAnonymizedPath(originalPath);
  expect(anonymizedPath).not.toBe(originalPath);
  expect(anonymizedPath).toBe('[path]');
});
