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

import * as _ from 'lodash';
import * as chalk from 'chalk';
import * as child_process from 'child_process';
import * as fs from 'fs';
import * as simplegit from 'simple-git/promise';
import * as util from 'util';
import * as yargs from 'yargs';

import findRepoRoot from '@rnw-scripts/find-repo-root';

const glob = util.promisify(require('glob').glob);

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
    const [rootPkg] = await enumeratePackages(
      pkgJson => pkgJson.name === 'react-native-windows-repo',
    );
    await updatePackage(rootPkg, {
      scripts: {change: `beachball change --branch ${branchName}`},
    });

    console.log('Updating package versions...');
    await updatePackageVersions(`${argv.rnVersion}.0-preview.0`);
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
  for (const packagePath of await enumeratePackagesToPromote()) {
    await updateBeachballConfig(packagePath, release, version);
  }
}

/**
 * Modifies beachball config to the right npm tag and version bump restrictions
 *
 * @param packagePath path of the package to update
 * @param release the release type
 * @param version major + minor version
 */
async function updateBeachballConfig(
  packagePath: string,
  release: ReleaseType,
  version: string,
) {
  switch (release) {
    case 'preview':
      return updatePackage(packagePath, {
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'patch'],
        },
      });

    case 'latest':
      return updatePackage(packagePath, {
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'prerelease'],
        },
      });

    case 'legacy':
      return updatePackage(packagePath, {
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'prerelease'],
        },
      });
  }
}

/**
 * Assign properties to the npm package of the given name
 * @param packagePath path path to a package
 * @param props key/values to merge into the package.json
 */
async function updatePackage(packagePath: string, props: {[key: string]: any}) {
  const packageJson = await jsonOfPackage(packagePath);

  _.merge(packageJson, props);
  await fs.promises.writeFile(
    packagePath,
    JSON.stringify(packageJson, null /*replacer*/, 2 /*space*/) + '\n',
  );
}

/**
 * Returns the JSON content of a package
 */
async function jsonOfPackage(packagePath: string): Promise<any> {
  const packageContent = await fs.promises.readFile(packagePath);
  if (!packageContent) {
    throw new Error(`Unable to read package file "${packagePath}"`);
  }

  return JSON.parse(packageContent.toString());
}

/**
 * Returns the name of a package
 */
async function nameOfPackage(packagePath: string): Promise<string> {
  return (await jsonOfPackage(packagePath)).name;
}

/**
 * Finds packages where we need to update version number + beachball config
 * @returns absolute paths to package.json
 */
async function enumeratePackagesToPromote(): Promise<string[]> {
  return enumeratePackages(pkgJson => pkgJson.promoteRelease === true);
}

/**
 * Finds packages matching a given predicate
 * @param pred predicate describing whether to match a package.json
 * @returns absolute paths to package.json
 */
async function enumeratePackages(
  pred: (pkgJson: any) => boolean = () => true,
): Promise<string[]> {
  const repoRoot = await findRepoRoot();
  const allPackages: string[] = await glob('**/package.json', {
    cwd: repoRoot,
    ignore: '**/node_modules/**',
    absolute: true,
  });

  const matchingPackages: string[] = [];
  for (const packageFile of allPackages) {
    const packageJson = await jsonOfPackage(packageFile);
    if (pred(packageJson)) {
      matchingPackages.push(packageFile);
    }
  }

  return matchingPackages;
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
  const promotedPackages = await Promise.all(
    packagesToPromote.map(nameOfPackage),
  );

  for (const packagePath of packagesToPromote) {
    await updatePackage(packagePath, {version});
  }

  // We need to update anything that might have a dependency on what we just
  // bumped.
  for (const packagePath of await enumeratePackages()) {
    for (const field of [
      'dependencies',
      'peerDependencies',
      'devDependencies',
    ]) {
      const dependencies = (await jsonOfPackage(packagePath))[field];
      if (!dependencies) {
        continue;
      }

      for (const dependencyPackage of Object.keys(dependencies)) {
        if (promotedPackages.includes(dependencyPackage)) {
          dependencies[dependencyPackage] = version;
        }
      }

      await updatePackage(packagePath, {[field]: dependencies});
    }
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
