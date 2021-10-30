/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 */

import glob from 'glob';
import {promises as fs} from 'fs';
import path from 'path';

import {copyProjectTemplateAndReplace} from '../generator-windows';
import {findPackage} from '@react-native-windows/package-utils';
import {usingScratchDirectory} from './sratchDirectory';

beforeAll(() => {
  console.log = jest.fn();
});

type TemplatePermutation = {
  name: string;
  language: 'cpp' | 'cs';
  projectType: 'app' | 'lib';
  experimentalNuGetDependency: boolean;
  useWinUI3: boolean;
  useHermes: boolean;
};

const permutations: TemplatePermutation[] = [];

for (const language of ['cpp', 'cs'] as const) {
  for (const projectType of ['app', 'lib'] as const) {
    for (const experimentalNuGetDependency of [false, true]) {
      for (const useWinUI3 of [false, true]) {
        for (const useHermes of [false, true]) {
          const friendlyLanguage = language === 'cpp' ? 'C++' : 'C#';
          const friendlyExperiments = [
            ...(experimentalNuGetDependency ? ['NuGet'] : []),
            ...(useWinUI3 ? ['WinUI3'] : []),
            ...(useHermes ? ['Hermes'] : []),
          ].join('+');
          const friendlyProjectType =
            projectType === 'app' ? 'Application' : 'Library';

          const name = `${friendlyLanguage} ${friendlyExperiments} ${friendlyProjectType}`;
          permutations.push({
            name,
            language,
            projectType,
            experimentalNuGetDependency,
            useWinUI3,
            useHermes,
          });
        }
      }
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
          ...permutation,
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
