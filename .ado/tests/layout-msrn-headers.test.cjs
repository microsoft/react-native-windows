// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

const assert = require('node:assert/strict');
const {readFileSync} = require('node:fs');
const path = require('node:path');
const {test} = require('node:test');

test('NuGet layout ships every header included by the public bridging umbrella', () => {
  const root = path.resolve(__dirname, '..', '..', 'vnext');
  const umbrella = readFileSync(
    path.join(root, 'include', 'React', 'Bridging.h'),
    'utf8',
  );
  const layout = readFileSync(
    path.join(root, 'Scripts', 'Tfs', 'Layout-MSRN-Headers.ps1'),
    'utf8',
  );
  const headers = [...umbrella.matchAll(/#include <react\/bridging\/([^>]+)>/g)];
  assert.ok(headers.length > 0, 'Expected public bridging includes');
  for (const [, header] of headers) {
    assert.ok(
      layout.includes(
        `Copy-Item -Force -Path $ReactNativeRoot\\ReactCommon\\react\\bridging\\${header} -Destination $MSRNCxxTargetRoot\\ReactCommon\\react\\bridging\\`,
      ),
      `NuGet layout is missing react/bridging/${header}`,
    );
  }
});

test('NuGet layout ships the debug umbrella used by public bridging headers', () => {
  const layout = readFileSync(
    path.resolve(__dirname, '../../vnext/Scripts/Tfs/Layout-MSRN-Headers.ps1'),
    'utf8',
  );
  for (const header of [
    'flags.h',
    'react_native_assert.h',
    'react_native_expect.h',
    'React\\Debug.h',
  ]) {
    const directory = header.includes('\\') ? 'React\\' : '';
    assert.ok(
      layout.includes(
        `Copy-Item -Force -Path $ReactNativeRoot\\ReactCommon\\react\\debug\\${header} -Destination $MSRNCxxTargetRoot\\ReactCommon\\react\\debug\\${directory}`,
      ),
      `NuGet layout is missing react/debug/${header}`,
    );
  }
});
