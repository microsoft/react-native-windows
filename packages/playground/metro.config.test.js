/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

const assert = require('node:assert/strict');
const path = require('node:path');
const test = require('node:test');
const config = require('./metro.config');

test('runs the resolved React Native environment before the sample entry', () => {
  const resolved = config.resolver.resolveRequest(
    {
      originModulePath: path.join(__dirname, 'Samples', 'simple.tsx'),
      resolveRequest: (_context, moduleName) => ({
        type: 'sourceFile',
        filePath: moduleName,
      }),
    },
    'react-native/setup-env',
    'windows',
  );
  const modules = config.serializer.getModulesRunBeforeMainModule();

  assert.ok(
    modules.includes(resolved.filePath),
    `Resolved setup module is missing from startup preludes: ${resolved.filePath}`,
  );
  assert.equal(modules.length, new Set(modules).size);
});
