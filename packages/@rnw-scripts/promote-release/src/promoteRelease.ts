/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * This script automatically changes files to prepare to move a release from
 * one stage to another. E.g. going from a release in our master branch to a
 * preview release in a stable branch, promoting a preview to latest, or moving
 * latest to legacy.
 *
 * See "How to promote a release" in our wiki for a detailed guide.
 *
 * @format
 */

import * as chalk from 'chalk';
import * as child_process from 'child_process';
import * as simplegit from 'simple-git/promise';
import * as yargs from 'yargs';

import {
  enumerateRepoPackages,
  WritableNpmPackage,
} from '@react-native-windows/package-utils';
import findRepoRoot from '@react-native-windows/find-repo-root';

type ReleaseType = 'preview' | 'latest' | 'legacy';

(async () => {
  const argv = collectArgs();
  const git = simplegit();
  const branchName = `${argv.rnVersion}-stable`;
  const commitMessage = `Promote ${argv.rnVersion} to ${argv.release}`;

  if (argv.release === 'preview') {
    console.log(`Creating branch ${branchName}...`);
    await git.checkoutBranch(branchName, 'HEAD');
  }

  console.log('Updating Beachball configuration...');
  await updateBeachballConfigs(argv.release as ReleaseType, argv.rnVersion);

  if (argv.release === 'preview') {
    console.log('Updating root change script...');
    const rootPkg = await WritableNpmPackage.fromPath(await findRepoRoot());
    if (!rootPkg) {
      throw new Error('Unable to find root npm package');
    }

    await rootPkg.mergeProps({
      scripts: {change: `beachball change --branch ${branchName}`},
    });

    console.log('Updating package versions...');
    await updatePackageVersions(`${argv.rnVersion}.0-preview.0`);

    console.log('Setting packages published from master as private...');
    await markMasterPackagesPrivate();
  }

  console.log('Committing changes...');
  await git.add('--all');
  await git.commit(commitMessage);

  console.log('Generating change files...');
  if (argv.release === 'preview') {
    await createChangeFiles('prerelease', commitMessage);
  } else {
    await createChangeFiles('patch', commitMessage);
  }

  console.log(chalk.green('All done! Please check locally commited changes.'));
})();

/**
 * Parse and validate program arguments
 */
function collectArgs() {
  const argv = yargs
    .options({
      release: {
        describe: 'What release channel to promote to',
        choices: ['preview', 'latest', 'legacy'],
        demandOption: true,
      },
      rnVersion: {
        describe: 'The semver major + minor version of the release (e.g. 0.62)',
        type: 'string',
        demandOption: true,
      },
    })
    .wrap(120)
    .version(false).argv;

  if (!argv.rnVersion.match(/\d+\.\d+/)) {
    console.error(chalk.red('Unexpected format for version (expected x.y)'));
    process.exit(1);
  }

  return argv;
}

/**
 * Modifies beachball configurations to the right npm tag and version bump
 * restrictions
 *
 * @param release the release type
 * @param version major + minor version
 */
async function updateBeachballConfigs(release: ReleaseType, version: string) {
  for (const pkg of await enumeratePackagesToPromote()) {
    await updateBeachballConfig(pkg, release, version);
  }
}

/**
 * Modifies beachball config to the right npm tag and version bump restrictions
 *
 * @param pkg the package to update
 * @param release the release type
 * @param version major + minor version
 */
async function updateBeachballConfig(
  pkg: WritableNpmPackage,
  release: ReleaseType,
  version: string,
) {
  switch (release) {
    case 'preview':
      return pkg.mergeProps({
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'patch'],
        },
      });

    case 'latest':
      return pkg.mergeProps({
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'prerelease'],
        },
      });

    case 'legacy':
      return pkg.mergeProps({
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'prerelease'],
        },
      });
  }
}

/**
 * Finds packages where we need to update version number + beachball config
 */
async function enumeratePackagesToPromote(): Promise<WritableNpmPackage[]> {
  return enumerateRepoPackages(async pkg => pkg.json.promoteRelease === true);
}

/**
 * Get the npm distribution tag for a given version
 *
 * @param release
 * @param version
 */
function distTag(release: ReleaseType, version: string): string {
  if (release === 'legacy') {
    return `v${version}-stable`;
  } else {
    return release;
  }
}

/**
 * Change the version of main packages to the given string
 * @param version The new package version
 */
async function updatePackageVersions(version: string) {
  const packagesToPromote = await enumeratePackagesToPromote();
  const promotedPackages = packagesToPromote.map(p => p.json.name);

  for (const pkg of packagesToPromote) {
    await pkg.mergeProps({version});
  }

  // We need to update anything that might have a dependency on what we just
  // bumped.
  for (const pkg of await enumerateRepoPackages()) {
    for (const field of [
      'dependencies',
      'peerDependencies',
      'devDependencies',
    ]) {
      const dependencies = pkg.json[field];
      if (!dependencies) {
        continue;
      }

      for (const dependencyPackage of Object.keys(dependencies)) {
        if (promotedPackages.includes(dependencyPackage)) {
          dependencies[dependencyPackage] = version;
        }
      }

      await pkg.mergeProps({[field]: dependencies});
    }
  }
}

/**
 * Sets all packages that are published from our master branch as private, to
 * avoid bumping and publishing them from our stable branch. Beachball will
 * ensure we do not depend on any of these in our published packages.
 */
async function markMasterPackagesPrivate() {
  const masterPublishedPackages = await enumerateRepoPackages(
    async pkg => !pkg.json.promoteRelease && !pkg.json.private,
  );

  for (const pkg of masterPublishedPackages) {
    await pkg.assignProps({private: true});
  }
}

/**
 * Create change files to do a version bump
 *
 * @param changeType prerelease or patch
 * @param message changelog message
 */
async function createChangeFiles(
  changeType: 'prerelease' | 'patch',
  message: string,
) {
  const repoRoot = await findRepoRoot();
  child_process.execSync(
    `npx beachball change --type ${changeType} --message "${message}"`,
    {cwd: repoRoot, stdio: 'ignore'},
  );
}
