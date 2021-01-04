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
  enumerateRepoPackages,
  findPackage,
  findRepoPackage,
} from '@react-native-windows/package-utils';
import runCommand from './runCommand';
import {upgradeOverrides} from 'react-native-platform-override';

/**
 * Describes the dependencies of a package
 */
export type PackageDeps = {
  packageName: string;
  dependencies?: Record<string, string>;
  peerDependencies?: Record<string, string>;
  devDependencies?: Record<string, string>;
};

/**
 * Describes the dependencies of a monorepo-local NPM package (with extra
 * information)
 */
export type LocalPackageDeps = PackageDeps & {
  outOfTreePlatform: boolean;
};

/**
 * Describes a package before and after being updated
 */
export type PackageDiff = {
  oldPackage: PackageDeps;
  newPackage: PackageDeps;
};

/**
 * List of package names for react-native out-of-tree platforms
 */
const OUT_OF_TREE_PLATFORMS = [
  '@office-iss/react-native-win32',
  'react-native-windows',
];

/**
 * Upgrades all packages in the monorepo cwd resides in to use a new version
 * of react-native, updating non-react-native dependencies, devDependencies,
 * and peerDependencies where appropriate.
 */
export default async function upgradeDependencies(
  newReactNativeVersion: string,
) {
  const reactNativeDiff = await upgradeReactNative(newReactNativeVersion);
  const repoConfigDiff = await upgradeRepoConfig(newReactNativeVersion);
  const localPackages = (await enumerateRepoPackages()).map(pkg => ({
    ...extractPackageDeps(pkg.json),
    outOfTreePlatform: OUT_OF_TREE_PLATFORMS.includes(pkg.json.name),
  }));

  const newDeps = calcPackageDependencies(
    newReactNativeVersion,
    reactNativeDiff,
    repoConfigDiff,
    localPackages,
  );

  const writablePackages = await enumerateRepoPackages();
  await Promise.all(
    newDeps.map(async deps => {
      const [writablePackage] = writablePackages.filter(
        p => p.json.name === deps.packageName,
      );

      const oldJson = writablePackage.json;
      const newJson = Object.assign(
        oldJson,
        _.pick(deps, 'dependencies', 'peerDependencies', 'devDependencies'),
      );

      // If all dependencies were deleted we want to delete the group from the
      // JSON, but still want to retain previous ordering
      if (!deps.dependencies) {
        delete newJson.dependencies;
      }
      if (!deps.devDependencies) {
        delete newJson.devDependencies;
      }
      if (!deps.peerDependencies) {
        delete newJson.peerDependencies;
      }

      await writablePackage.setJson(newJson);
    }),
  );

  await runCommand('yarn install');
}

/**
 * Updates and installs a new version of the literal react-native package
 * across the monorepo
 */
async function upgradeReactNative(
  newReactNativeVersion: string,
): Promise<PackageDiff> {
  const platformPackages = await enumerateRepoPackages(async pkg =>
    OUT_OF_TREE_PLATFORMS.includes(pkg.json.name),
  );

  if (platformPackages.length === 0) {
    throw new Error(
      'Expected to find a package for at least 1 out-of-tree platform',
    );
  }

  const findRnOpts = {searchPath: platformPackages[0].path};
  const origJson = (await findPackage('react-native', findRnOpts))!.json;

  for (const pkg of await enumerateRepoPackages()) {
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

    if (pkg.json.devDependencies && pkg.json.devDependencies['react-native']) {
      await pkg.mergeProps({
        devDependencies: {
          'react-native': bumpSemver(
            pkg.json.devDependencies['react-native'],
            newReactNativeVersion,
          ),
        },
      });
    }
  }

  await runCommand('yarn install');
  const newJson = (await findPackage('react-native', findRnOpts))!.json;

  return extractPackageDiff(origJson, newJson);
}

/**
 * Uses override tooling to pull in a new version of the repo-config package
 */
async function upgradeRepoConfig(
  newReactNativeVersion: string,
): Promise<PackageDiff> {
  const origPackage = (await findRepoPackage('@react-native/repo-config'))!;

  const upgradeResults = await upgradeOverrides(
    path.join(origPackage.path, 'overrides.json'),
    {
      reactNativeVersion: newReactNativeVersion,
      allowConflicts: false,
    },
  );

  if (!upgradeResults.every(result => result.filesWritten)) {
    throw new Error(
      'Could not sync repo-config package due to conflicts. Please resolve manually',
    );
  }

  const newPackage = (await findRepoPackage('@react-native/repo-config'))!;
  return extractPackageDiff(origPackage.json, newPackage.json);
}

/**
 * Extracts old and new dependencies from old and new package.json
 */
function extractPackageDiff(origJson: any, newJson: any): PackageDiff {
  return {
    oldPackage: extractPackageDeps(origJson),
    newPackage: extractPackageDeps(newJson),
  };
}

/**
 * Extracts dependencies from package.json
 */
function extractPackageDeps(json: any): PackageDeps {
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

/**
 * Given inputs on the packages of the monorepo along with old + new RN related
 * packages, determines the list of dependencies for every package in the
 * monorepo. Exported for testability. Assumes all dependencies and
 * devDependencies have already been updated to the newer version of
 * react-native.
 */
export function calcPackageDependencies(
  newReactNativeVersion: string,
  reactNativePackageDiff: PackageDiff,
  repoConfigPackageDiff: PackageDiff,
  localPackages: LocalPackageDeps[],
): LocalPackageDeps[] {
  return localPackages.map(pkg => {
    const newPackage: LocalPackageDeps = _.cloneDeep(pkg);

    if (newPackage.outOfTreePlatform) {
      syncReactNativeDependencies(newPackage, reactNativePackageDiff);
    }

    if (newPackage.dependencies && newPackage.dependencies['react-native']) {
      newPackage.dependencies['react-native'] = bumpSemver(
        newPackage.dependencies['react-native'],
        newReactNativeVersion,
      );
    }

    ensureValidReactNativePeerDep(newPackage, newReactNativeVersion);
    ensureReactNativePeerDepsSatisfied(
      newPackage,
      reactNativePackageDiff.newPackage,
    );
    syncDevDependencies(
      newPackage,
      repoConfigPackageDiff,
      reactNativePackageDiff,
    );

    if (newPackage.dependencies) {
      newPackage.dependencies = sortByKeys(newPackage.dependencies);
    }
    if (newPackage.peerDependencies) {
      newPackage.peerDependencies = sortByKeys(newPackage.peerDependencies);
    }
    if (newPackage.devDependencies) {
      newPackage.devDependencies = sortByKeys(newPackage.devDependencies);
    }

    return newPackage;
  });
}

/**
 * Sort an object by keys
 */
function sortByKeys<T>(obj: Record<string, T>): Record<string, T> {
  return _(obj)
    .toPairs()
    .sortBy(0)
    .fromPairs()
    .value();
}

/**
 * Matches dependencies + peer dependencies of an out-of-tree platform to
 * those used by react-native.
 */
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
  if (Object.keys(newDeps).length === 0) {
    delete pkg.dependencies;
  } else {
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
  if (Object.keys(newPeerDeps).length === 0) {
    delete pkg.peerDependencies;
  } else {
    pkg.peerDependencies = newPeerDeps;
  }
}

/**
 * Updates devDependencies of all packages to align with those used in the RN
 * core monorepo where newer.
 */
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
      if (
        semver.gt(
          semver.minVersion(newRNDevDevDeps[dependency])!,
          semver.minVersion(version)!,
        )
      ) {
        pkg.devDependencies![dependency] = newRNDevDevDeps[dependency];
      }
    } else if (newRepoConfigDeps.hasOwnProperty(dependency)) {
      if (
        semver.gt(
          semver.minVersion(newRepoConfigDeps[dependency])!,
          semver.minVersion(version)!,
        )
      ) {
        pkg.devDependencies![dependency] = newRepoConfigDeps[dependency];
      }
    }
  }
}

/**
 * Ensures any peer-dependencies in a package referencing react-native remain
 * compatible with the version we're upgrading too.
 */
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
    // Semver satisfaction logic for * is too strict, so we need to special
    // case it https://github.com/npm/node-semver/issues/130
    pkg.peerDependencies['react-native'] === '*' ||
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
 * Ensure that a package fulfills peer depenedncies for react-native if relying on it
 */
function ensureReactNativePeerDepsSatisfied(
  pkg: LocalPackageDeps,
  newReactNativePkg: PackageDeps,
) {
  if (!pkg.dependencies || !pkg.dependencies['react-native']) {
    return;
  }

  const rnPeerDeps = newReactNativePkg.peerDependencies || {};
  for (const [dep, rnDepVersion] of Object.entries(rnPeerDeps)) {
    if (!pkg.dependencies[dep]) {
      pkg.dependencies[dep] = rnDepVersion;
    } else if (
      // Semver satisfaction logic for * is too strict, so we need to special
      // case it https://github.com/npm/node-semver/issues/130
      rnDepVersion !== '*' &&
      !semver.satisfies(pkg.dependencies[dep], rnDepVersion)
    ) {
      pkg.dependencies[dep] = bumpSemver(pkg.dependencies[dep], rnDepVersion);
    }
  }
}

/**
 * Updates semver version or range to use a new version, preserving previous
 * range semantics. Only simple cases are supported.
 *
 * @param origVersion the original semver range/value
 * @param newVersion the new value to bump to
 */
function bumpSemver(origVersion: string, newVersion: string): string {
  if (!semver.valid(origVersion) && !semver.validRange(origVersion)) {
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
