/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as chalk from 'chalk';
import {rm, cp} from 'shelljs';

import {
  WritableNpmPackage,
  findPackage,
} from '@react-native-windows/package-utils';

(async () => {
  await applyRnPatches();
})();

async function applyRnPatches() {
  // Replace community CLI version of metro packages with our own, since the
  // CLI used by RN nightlies pulls in a mismatched version
  const cliDepsToReplace = [
    'metro',
    'metro-config',
    'metro-core',
    'metro-react-native-babel-transformer',
    'metro-resolver',
    'metro-runtime',
  ];

  for (const dep of cliDepsToReplace) {
    await replaceCliDependency(dep);
  }
}

async function replaceCliDependency(dependencyName: string) {
  const rnPackage = await findPackage('react-native', {
    searchPath: process.cwd(),
  });
  if (!rnPackage) {
    // We may not have a hoisted RN package above us to go off of
    return;
  }

  const cliPackage = await findPackage('@react-native-community/cli', {
    searchPath: rnPackage.path,
  });
  if (!cliPackage) {
    terminateWithError(
      'Unable to locate "@react-native-community/cli" package',
    );
  }

  const localDep = await findPackage(dependencyName, {searchPath: __dirname});
  if (!localDep) {
    terminateWithError(
      `Expected to find '@react-native-windows/cli' dependency: '${dependencyName}'`,
    );
  }

  const cliDep = await findPackage(dependencyName, {
    searchPath: cliPackage.path,
  });
  if (!cliDep) {
    terminateWithError(
      `Expected to find '@react-native-community/cli' CLI dependency: '${dependencyName}'`,
    );
  }

  if (
    cliPackage.json?.dependencies?.[dependencyName] === localDep.json.version
  ) {
    return;
  }

  const writableCliPackage = await WritableNpmPackage.fromPath(cliPackage.path);
  await writableCliPackage!.mergeProps({
    dependencies: {
      [dependencyName]: localDep.json.version,
    },
  });

  rm('-rf', cliDep.path);
  cp('-r', localDep.path, cliDep.path);
}

function terminateWithError(err: string): never {
  console.error(chalk.red(err));
  process.exit(1);
}
