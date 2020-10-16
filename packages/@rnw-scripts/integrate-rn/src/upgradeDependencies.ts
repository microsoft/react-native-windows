/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as path from 'path';
import * as semver from 'semver';
import {
  enumerateLocalPackages,
  findPackage,
  findLocalPackage,
} from '@rnw-scripts/package-utils';
import runCommand from './runCommand';
import {upgradeOverrides} from 'react-native-platform-override';

export type PackageDeps = {
  packageName: string;
  dependencies?: Record<string, string>;
  peerDependencies?: Record<string, string>;
  devDependencies?: Record<string, string>;
};

export type LocalPackageDeps = PackageDeps & {
  outOfTreePlatform: boolean;
};

export type PackageDiff = {
  oldPackage: PackageDeps;
  newPackage: PackageDeps;
};

const OUT_OF_TREE_PLATFORMS = [
  '@office-iss/react-native-win32',
  'react-native-windows',
];

export async function upgradeDependencies(newReactNativeVersion: string) {
  const reactNativeDiff = await upgradeReactNative(newReactNativeVersion);
  const repoConfigDiff = await upgradeRepoConfig(newReactNativeVersion);
  const localPackages = (await enumerateLocalPackages()).map(pkg => ({
    ...jsonPackageDeps(pkg.json),
    outOfTreePlatform: OUT_OF_TREE_PLATFORMS.includes(pkg.json.name),
  }));

  const newDeps = await determineNewPackageDependencies(
    newReactNativeVersion,
    reactNativeDiff,
    repoConfigDiff,
    localPackages,
  );

  const writablePackages = await enumerateLocalPackages();
  await Promise.all(
    newDeps.map(async deps => {
      const [writablePackage] = writablePackages.filter(
        p => p.json.name === deps.packageName,
      );
      await writablePackage.assignProps(
        _.pick(deps, 'dependencies', 'peerDependencies', 'devDependencies'),
      );
    }),
  );

  await runCommand('yarn install');
}

async function upgradeReactNative(
  newReactNativeVersion: string,
): Promise<PackageDiff> {
  const platformPackages = await enumerateLocalPackages(async pkg =>
    OUT_OF_TREE_PLATFORMS.includes(pkg.json.name),
  );

  if (platformPackages.length === 0) {
    throw new Error(
      'Expected to find a package for at least 1 out-of-tree platform',
    );
  }

  const findRnOpts = {searchPath: platformPackages[0].path};
  const origJson = (await findPackage('react-native', findRnOpts))!.json;

  for (const pkg of await enumerateLocalPackages()) {
    if (pkg.json.dependencies && pkg.json.dependencies['react-native']) {
      await pkg.mergeProps({
        dependencies: {
          'react-native': bumpSemver(
            pkg.json.dependencies['react-native'],
            newReactNativeVersion,
          ),
        },
      });
    }
  }

  await runCommand('yarn install');
  const newJson = (await findPackage('react-native', findRnOpts))!.json;

  return jsonPackageDiff(origJson, newJson);
}

async function upgradeRepoConfig(
  newReactNativeVersion: string,
): Promise<PackageDiff> {
  const origPackage = (await findLocalPackage('@react-native/repo-config'))!;

  const [upgradeResult] = await upgradeOverrides(
    path.join(origPackage.path, 'overrides.json'),
    {
      reactNativeVersion: newReactNativeVersion,
      allowConflicts: false,
    },
  );

  if (upgradeResult && !upgradeResult.filesWritten) {
    throw new Error(
      'Could not sync repo-config package due to conflicts. Please resolve manually',
    );
  }

  const newPackage = (await findLocalPackage('@react-native/repo-config'))!;
  return jsonPackageDiff(origPackage.json, newPackage.json);
}

function jsonPackageDiff(origJson: any, newJson: any): PackageDiff {
  return {
    oldPackage: jsonPackageDeps(origJson),
    newPackage: jsonPackageDeps(newJson),
  };
}

function jsonPackageDeps(json: any): PackageDeps {
  return {
    packageName: json.name,
    ...(json.dependencies && {dependencies: json.dependencies}),
    ...(json.peerDependencies && {
      peerDependencies: json.peerDependencies,
    }),
    ...(json.devDependencies && {
      devDependencies: json.devDependencies,
    }),
  };
}

export function determineNewPackageDependencies(
  newReactNativeVersion: string,
  reactNativePackageDiff: PackageDiff,
  repoConfigPackageDiff: PackageDiff,
  localPackages: LocalPackageDeps[],
): LocalPackageDeps[] {
  return localPackages.map(pkg => {
    const newPackage: LocalPackageDeps = _.clone(pkg);

    if (pkg.outOfTreePlatform) {
      syncReactNativeDependencies(newPackage, reactNativePackageDiff);
    }

    if (pkg.dependencies && pkg.dependencies['react-native']) {
      pkg.dependencies['react-native'] = bumpSemver(
        pkg.dependencies['react-native'],
        newReactNativeVersion,
      );
    }

    ensureValidReactNativePeerDep(pkg, newReactNativeVersion);
    syncDevDependencies(pkg, repoConfigPackageDiff, reactNativePackageDiff);

    pkg.dependencies = pkg.dependencies && sortKeys(pkg.dependencies);
    pkg.peerDependencies =
      pkg.peerDependencies && sortKeys(pkg.peerDependencies);
    pkg.devDependencies = pkg.devDependencies && sortKeys(pkg.devDependencies);

    return newPackage;
  });
}

/**
 * Sort an object by keys
 */
function sortKeys<T>(obj: Record<string, T>): Record<string, T> {
  return _(obj)
    .toPairs()
    .sortBy(0)
    .fromPairs()
    .value();
}

function syncReactNativeDependencies(
  pkg: LocalPackageDeps,
  reactNativePackageDiff: PackageDiff,
) {
  // Because we host JS from RN core, we need to provide all of the
  // dependencies and peerDependencies declared by react-native. Pick all of
  // them, + any extra dependencies we previously had compared to core (e.g.
  // @react-native-windows/cli)
  const extraDeps = _.difference(
    Object.keys(pkg.dependencies || {}),
    Object.keys(reactNativePackageDiff.oldPackage.dependencies || {}),
  );
  const newDeps = {
    ..._.pick(pkg.dependencies, extraDeps),
    ...reactNativePackageDiff.newPackage.dependencies,
  };
  if (Object.keys(newDeps).length !== 0) {
    pkg.dependencies = newDeps;
  }

  const extraPeerDeps = _.difference(
    Object.keys(pkg.peerDependencies || {}),
    Object.keys(reactNativePackageDiff.oldPackage.peerDependencies || {}),
  );
  const newPeerDeps = {
    ..._.pick(pkg.peerDependencies, extraPeerDeps),
    ...reactNativePackageDiff.newPackage.peerDependencies,
  };
  if (Object.keys(newPeerDeps).length !== 0) {
    pkg.peerDependencies = newPeerDeps;
  }
}

function syncDevDependencies(
  pkg: LocalPackageDeps,
  repoConfigPackageDiff: PackageDiff,
  reactNativePackageDiff: PackageDiff,
) {
  // We don't need all of the dev dependencies from the RN repo in our
  // packages. We instead just make sure to update our own dev dependencies if
  // the RN repo has the same package of a newer version

  const devDependencies = Object.entries(pkg.devDependencies || {});
  const newRNDevDevDeps =
    reactNativePackageDiff.newPackage.devDependencies || {};
  const newRepoConfigDeps = repoConfigPackageDiff.newPackage.dependencies || {};

  for (const [dependency, version] of devDependencies) {
    if (pkg.outOfTreePlatform && newRNDevDevDeps.hasOwnProperty(dependency)) {
      if (semver.gt(newRNDevDevDeps[dependency], version)) {
        pkg.devDependencies![dependency] = newRNDevDevDeps[dependency];
      }
    } else if (newRepoConfigDeps.hasOwnProperty(dependency)) {
      if (semver.gt(newRepoConfigDeps[dependency], version)) {
        pkg.devDependencies![dependency] = newRepoConfigDeps[dependency];
      }
    }
  }
}

function ensureValidReactNativePeerDep(
  pkg: LocalPackageDeps,
  newReactNativeVersion: string,
) {
  if (!(pkg.peerDependencies && pkg.peerDependencies['react-native'])) {
    return;
  }

  // If we have a range, such as in our stable branches, only bump if needed,
  // as changing the peer depenedncy is a breaking change.
  if (
    semver.satisfies(
      newReactNativeVersion,
      pkg.peerDependencies['react-native'],
    )
  ) {
    return;
  }

  // Use a heuristic to bump otherwise
  pkg.peerDependencies['react-native'] = bumpSemver(
    pkg.peerDependencies['react-native'],
    newReactNativeVersion,
  );
}

/**
 * Updates semver version or range to use a new version, preserving previous
 * range semantics. Only simple cases are supported.
 *
 * @param origVersion the original semver range/value
 * @param newVersion the new value to bump to
 */
function bumpSemver(origVersion: string, newVersion: string): string {
  if (!semver.valid(origVersion) || !semver.validRange(origVersion)) {
    throw new Error(`Unable to bump invalid semver '${origVersion}'`);
  }

  // Semver allows multiple ranges, hypen ranges, star ranges, etc. Don't try
  // to reason about how to bump all of those and just bail if we see them.
  const simpleSemver = /([\^~]?)(\d+\.\d+(\.\d+)?(-\w+\.\d+)?)/;
  if (!simpleSemver.test(origVersion)) {
    throw new Error(`Unable to bump complicated semver '${origVersion}'`);
  }

  if (origVersion.startsWith(`~`) || origVersion.startsWith('^')) {
    return `${origVersion[0]}${newVersion}`;
  } else {
    return newVersion;
  }
}
