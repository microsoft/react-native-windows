/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import DiffStrategy, {DiffStrategies} from '../DiffStrategy';
import {acquireGitRepo, usingFiles} from './Resource';
import GitReactFileRepository from '../GitReactFileRepository';

let gitReactRepo: GitReactFileRepository;
let disposeReactRepo: () => Promise<void>;

beforeAll(async () => {
  [gitReactRepo, disposeReactRepo] = await acquireGitRepo();
});

afterAll(async () => {
  await disposeReactRepo();
});

test('assumeSame', async () => {
  const overrideFile = '0.62.2/flowconfig.pristine';
  const diff = await evaluateStrategy(
    DiffStrategies.asssumeSame(),
    overrideFile,
  );
  expect(diff).toBe('');
});

test('compareBaseFile - No Difference', async () => {
  const overrideFile = '0.62.2/flowconfig.pristine';
  const diff = await evaluateStrategy(
    DiffStrategies.compareBaseFile(overrideFile, '.flowconfig', '0.62.2'),
    overrideFile,
  );
  expect(diff).toBe('');
});

test('compareBaseFile - Difference', async () => {
  const overrideFile = '0.62.2/flowconfig.windows.addition';
  const diff = await evaluateStrategy(
    DiffStrategies.compareBaseFile(overrideFile, '.flowconfig', '0.62.2'),
    overrideFile,
  );
  expect(diff).toBe(
    `@@ -15,6 +15,8 @@
 ; require from fbjs/lib instead: require('fbjs/lib/warning')
 .*/node_modules/warning/.*
 
+; This change is Windows only
+
 ; Flow doesn't support platforms
 .*/Libraries/Utilities/LoadingView.js`,
  );
});

async function evaluateStrategy(
  strategy: DiffStrategy,
  overrideFile: string,
): Promise<string> {
  return usingFiles([overrideFile], async overrideRepo => {
    return strategy.diff(gitReactRepo, overrideRepo);
  });
}
