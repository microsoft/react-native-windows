/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as path from 'path';
import dumpVisualTree, {VisualTree} from '../dumpVisualTree';

const assetPath = '@react-native-windows/tester/js/assets/flux@3x.png';
const packagesPath = path.resolve(__dirname, '../../../..');

function bundledImageUri(platform: string, marker: string): string {
  return `file://${packagesPath}\\e2e-test-app-fabric\\windows\\RNTesterApp-Fabric.Package\\bin\\${platform}\\Release\\AppX\\RNTesterApp-Fabric\\Bundle\\assets/${marker}${assetPath}`;
}

function mockVisualTree(tree: VisualTree) {
  Object.defineProperty(global, 'automationClient', {
    configurable: true,
    value: {
      invoke: async () => ({type: 'success', result: tree}),
    },
  });
}

async function dumpImageSource(
  uri: string,
  options?: Parameters<typeof dumpVisualTree>[1],
): Promise<string | undefined> {
  const tree: VisualTree = {
    'Automation Tree': {},
    'Component Tree': {Type: 'Image', _Props: {Sources: [{Uri: uri}]}},
    'Visual Tree': {},
  };
  mockVisualTree(tree);
  await dumpVisualTree('image', options);
  return tree['Component Tree']._Props?.Sources?.[0].Uri;
}

afterEach(() => {
  Reflect.deleteProperty(global, 'automationClient');
});

describe.each(['x64', 'x86'])('bundled image snapshots (%s)', platform => {
  test.each(['@@/', '@@/@@/', '_', '___'])(
    'normalizes packaged image paths with the %s marker',
    async marker => {
      expect(await dumpImageSource(bundledImageUri(platform, marker))).toBe(
        `<localOrBundlerUri>${assetPath}`,
      );
    },
  );
});

test('normalizes image sources in nested component trees', async () => {
  const tree: VisualTree = {
    'Automation Tree': {},
    'Component Tree': {
      Type: 'View',
      __Children: [
        {
          Type: 'Image',
          _Props: {Sources: [{Uri: bundledImageUri('x64', '@@/')}]},
        },
      ],
    },
    'Visual Tree': {},
  };
  mockVisualTree(tree);

  await dumpVisualTree('view');

  expect(tree['Component Tree'].__Children?.[0]._Props?.Sources?.[0].Uri).toBe(
    `<localOrBundlerUri>${assetPath}`,
  );
});

test('preserves bundled image paths when normalization is disabled', async () => {
  const uri = bundledImageUri('x64', '@@/');
  expect(await dumpImageSource(uri, {removeGuidsFromImageSources: false})).toBe(
    uri,
  );
});

test.each([
  `file://C:\\other-app\\Bundle\\assets/@@/${assetPath}`,
  `https://example.com/assets/@@/${assetPath}`,
])('preserves unrelated image URI %s', async uri => {
  expect(await dumpImageSource(uri)).toBe(uri);
});
