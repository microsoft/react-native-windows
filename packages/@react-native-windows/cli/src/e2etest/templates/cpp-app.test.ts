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

type TemplatePermutation = {
  name: string;
  experimentalNuGetDependency: boolean;
  useWinUI3: boolean;
  useHermes: boolean;
};

const permutations: TemplatePermutation[] = [];

for (const experimentalNuGetDependency of [false, true]) {
  for (const useWinUI3 of [false, true]) {
    for (const useHermes of [false, true]) {
      const friendlyExperiments = [
        ...(experimentalNuGetDependency ? ['NuGet'] : []),
        ...(useWinUI3 ? ['WinUI3'] : []),
        ...(useHermes ? ['Hermes'] : []),
      ].join(' + ');
      const name =
        friendlyExperiments.length === 0
          ? 'cpp-app'
          : `cpp-app ${friendlyExperiments}`;
      permutations.push({
        name,
        experimentalNuGetDependency,
        useWinUI3,
        useHermes,
      });
    }
  }
}

for (const permutation of permutations) {
  test(permutation.name, async () => {
    const rnwPackage = await findPackage('react-native-windows');
    const templateRoot = path.join(rnwPackage!.path, 'template');

    await usingScratchDirectory(async dir => {
      await copyProjectTemplateAndReplace(
        templateRoot,
        dir,
        'SnapshotApp',
        'SnapshotApp',
        {
          projectGuid: '00000000-0000-0000-0000-000000000000',
          ...permutation,
          language: 'cpp',
          projectType: 'app',
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
}
