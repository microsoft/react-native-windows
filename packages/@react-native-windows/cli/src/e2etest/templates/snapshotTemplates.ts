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

type TemplatePermutation = {
  name: string;
  experimentalNuGetDependency: boolean;
  useWinUI3: boolean;
  useHermes: boolean;
};

export default function snapshotTemplate(
  language: 'cpp' | 'cs',
  projectType: 'app' | 'lib',
) {
  // eslint-disable-next-line no-undef
  console.log = jest.fn();

  const permutations =
    projectType === 'app'
      ? generateAppPermutations(language)
      : generateLibPermutations(language);

  for (const permutation of permutations) {
    // eslint-disable-next-line no-undef
    test(permutation.name, async () => {
      const rnwPackage = await findPackage('react-native-windows');
      const templateRoot = path.join(rnwPackage!.path, 'template');
      const name = projectType === 'app' ? 'SnapshotApp' : 'SnapshotLib';

      await usingScratchDirectory(async dir => {
        await copyProjectTemplateAndReplace(templateRoot, dir, name, name, {
          projectGuid: '00000000-0000-0000-0000-000000000000',
          ...permutation,
          language,
          projectType,
          overwrite: true,
          verbose: false,
          useDevMode: false,
          telemetry: false,
        });

        const files = glob
          .sync('**', {cwd: dir, absolute: true, nodir: true})
          .filter(f => !f.endsWith('.png'));
        for (const file of files) {
          const content = (await fs.readFile(file)).toString();

          // Other non-deterministic guids than project show up. Zero them out
          // for now.
          const neutralContent = content
            .replace(
              /[A-Fa-f0-9]{8}-([A-Fa-f0-9]{4}-){3}[A-Fa-f0-9]{12}/g,
              '00000000-0000-0000-0000-000000000000',
            )
            .replace(/Publisher="CN=[^"]+"/g, 'Publisher="CN=test"')
            .replace(
              /<PublisherDisplayName>[^<]+/g,
              '<PublisherDisplayName>test',
            );

          // eslint-disable-next-line no-undef
          expect(neutralContent).toMatchSnapshot(
            path.relative(dir, file).replace(/\\/g, '/'),
          );
        }
      });
    });
  }
}

function generateAppPermutations(
  language: 'cpp' | 'cs',
): TemplatePermutation[] {
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
            ? `${language}-app`
            : `${language}-app ${friendlyExperiments}`;
        permutations.push({
          name,
          experimentalNuGetDependency,
          useWinUI3,
          useHermes,
        });
      }
    }
  }

  return permutations;
}

function generateLibPermutations(
  language: 'cpp' | 'cs',
): TemplatePermutation[] {
  return [
    {
      name: `${language}-lib`,
      experimentalNuGetDependency: false,
      useWinUI3: false,
      useHermes: false,
    },
  ];
}
