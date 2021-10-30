/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 */

import glob from 'glob';
import {promises as fs} from 'fs';
import path from 'path';

import {copyProjectTemplateAndReplace} from '../../generator-windows';
import {findPackage} from '@react-native-windows/package-utils';
import {usingScratchDirectory} from '../sratchDirectory';

beforeAll(() => {
  console.log = jest.fn();
});

test('cpp-lib', async () => {
  const rnwPackage = await findPackage('react-native-windows');
  const templateRoot = path.join(rnwPackage!.path, 'template');

  await usingScratchDirectory(async dir => {
    await copyProjectTemplateAndReplace(
      templateRoot,
      dir,
      'SnapshotLib',
      'SnapshotLib',
      {
        projectGuid: '00000000-0000-0000-0000-000000000000',
        language: 'cpp',
        projectType: 'lib',
        experimentalNuGetDependency: false,
        useHermes: false,
        useWinUI3: false,
        overwrite: true,
        verbose: false,
        useDevMode: false,
        telemetry: false,
      },
    );

    const files = glob.sync('**', {cwd: dir, absolute: true, nodir: true});
    for (const file of files) {
      const content = (await fs.readFile(file)).toString();
      expect(content).toMatchSnapshot(path.basename(file));
    }
  });
});
