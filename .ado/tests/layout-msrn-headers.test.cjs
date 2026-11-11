// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

const assert = require('node:assert/strict');
const {existsSync, readFileSync, readdirSync} = require('node:fs');
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
  const headers = [
    ...umbrella.matchAll(/#include <react\/bridging\/([^>]+)>/g),
  ];
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

test('copied ReactCommon sources retain their React header dependency closure', () => {
  const root = path.resolve(__dirname, '../..');
  const layout = readFileSync(
    path.join(root, 'vnext/Scripts/Tfs/Layout-MSRN-Headers.ps1'),
    'utf8',
  );
  const files = new Map();
  for (const [, source, destination] of layout.matchAll(
    /^Copy-Item -Force -Path \$ReactNativeRoot\\ReactCommon\\(\S+) -Destination \$MSRNCxxTargetRoot\\(\S+)/gm,
  )) {
    const sourcePath = path.join(
      root,
      'node_modules/react-native/ReactCommon',
      source.replaceAll('\\', '/'),
    );
    const names = source.endsWith('*.h')
      ? readdirSync(path.dirname(sourcePath)).filter(name =>
          name.endsWith('.h'),
        )
      : [path.basename(sourcePath)];
    for (const name of names) {
      files.set(
        path.normalize(path.join(destination.replaceAll('\\', '/'), name)),
        path.join(path.dirname(sourcePath), name),
      );
    }
  }
  assert.ok(files.size > 20, 'Expected the staged ReactCommon source list');
  const visited = new Set();
  const visit = (name, source) => {
    if (visited.has(name)) {
      return;
    }
    visited.add(name);
    for (const [, quote, header] of readFileSync(source, 'utf8').matchAll(
      /^\s*#include\s*([<"])([^>"]+)[>"]/gm,
    )) {
      const candidates = [
        ...(quote === '"' ? [path.join(path.dirname(name), header)] : []),
        header,
        path.join('ReactCommon', header),
      ].map(candidate => path.normalize(candidate));
      const packaged = candidates.find(candidate => files.has(candidate));
      if (packaged) {
        visit(packaged, files.get(packaged));
      } else if (header.startsWith('React/')) {
        const forwarder = path.join(root, 'vnext/include', header);
        assert.ok(existsSync(forwarder), `${name} needs ${header}`);
        visit(header, forwarder);
      } else if (/^(ReactCommon|react|callinvoker)\//.test(header)) {
        assert.fail(
          `${name} includes ${header}, which is absent from the C++ package layout`,
        );
      }
    }
  };
  for (const [name, source] of files) {
    visit(name, source);
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
