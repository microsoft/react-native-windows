/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import path from 'path';
import {execSync} from 'child_process';

import * as projectUtils from '../utils/projectUtils';

test('getProjectId() does not match project name', () => {
  const projectName = 'test-project';
  expect(projectUtils.getProjectId(projectName)).not.toContain(projectName);
});

test('getProjectId() does not change for same project name', () => {
  const projectName = 'test-project';
  expect(projectUtils.getProjectId(projectName)).toBe(
    projectUtils.getProjectId(projectName),
  );
});

test('getProjectId() returns different ids for different project names', () => {
  const projectName1 = 'test-project1';
  const projectName2 = 'test-project2';
  expect(projectUtils.getProjectId(projectName1)).not.toBe(
    projectUtils.getProjectId(projectName2),
  );
});

test('usesReactNativeConfig() is false with no react-native.config.js present', async () => {
  const projectRoot = path.resolve(__dirname, 'projects/BlankUsesTypeScript');
  const value = await projectUtils.usesReactNativeConfig(projectRoot);
  expect(value).toBe(false);
});

test('usesReactNativeConfig() is true with react-native.config.js present', async () => {
  const projectRoot = path.resolve(
    __dirname,
    'projects/BlankUsesReactNativeConfig',
  );
  const value = await projectUtils.usesReactNativeConfig(projectRoot);
  expect(value).toBe(true);
});

test('usesTypeScript() is false with no tsconfig.json present', async () => {
  const projectRoot = path.resolve(
    __dirname,
    'projects/BlankUsesReactNativeConfig',
  );
  const value = await projectUtils.usesTypeScript(projectRoot);
  expect(value).toBe(false);
});

test('usesTypeScript() is true with tsconfig.json present', async () => {
  const projectRoot = path.resolve(__dirname, 'projects/BlankUsesTypeScript');
  const value = await projectUtils.usesTypeScript(projectRoot);
  expect(value).toBe(true);
});

test('configToProjectInfo() works with playground project info', async () => {
  const playgroundRoot = path.resolve(__dirname, '../../../../playground');
  const config = JSON.parse(
    execSync('npx @react-native-community/cli config', {
      cwd: playgroundRoot,
      stdio: ['ignore', 'pipe', 'ignore'],
    }).toString(),
  );

  const info = await projectUtils.configToProjectInfo(config);
  expect(info).not.toBeNull();

  const projectInfo = info as projectUtils.AppProjectInfo;

  expect(projectInfo.id).toBe(
    projectUtils.getProjectId('@rnw-scripts/playground'),
  );
  expect(projectInfo.platforms).toStrictEqual(['windows']);
  expect(projectInfo.rnwLang).toBe('cpp');
  expect(projectInfo.usesTS).toBe(true);
  expect(projectInfo.usesRNConfig).toBe(true);
  expect(projectInfo.jsEngine).toBe('Hermes');
  expect(projectInfo.rnwSource).toBe('Source');
}, 200000 /* Increase timeout of this test */);
