/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * This script will try to associate RNW changelog information with a GitHub
 * release, filling in any tag which does not yet have release information.
 *
 * @format
 * @ts-check
 */

import _ from 'lodash';
import chalk from 'chalk';
import fs from 'fs';
import path from 'path';
import semver from 'semver';
import simplegit from 'simple-git/promise';
import util from 'util';
import yargs from 'yargs';

import {Octokit} from '@octokit/rest';
import findRepoRoot from '@react-native-windows/find-repo-root';

const glob = util.promisify(require('glob').glob);

const RNW_REPO = {
  owner: 'microsoft',
  repo: 'react-native-windows',
};

/**
 * Representation the JSON chanelog comment obejct
 */
interface Comment {
  comment: string;
  author: string;
  commit: string;
  package: string;
}

/**
 * Representation of JSON changelog
 */
interface Changelog {
  name: string;
  entries: Array<{
    date: string;
    tag: string;
    version: string;
    comments: {[type in 'none' | 'prerelease' | 'minor' | 'major']: Comment[]};
  }>;
}

/**
 * Represents the details of an individual release
 */
interface Release {
  packageName: string;
  tag: string;
  version: semver.SemVer;
  comments: Comment[];
}

const argv = yargs
  .options({
    authToken: {
      describe: 'GitHub Auth token',
      type: 'string',
      demandOption: true,
    },
    yes: {
      alias: 'y',
      describe: 'Automatically confirm any warnings',
      type: 'boolean',
      demandOption: false,
    },
  })
  .version(false).argv;

const octokit = new Octokit({
  auth: argv.authToken,
  userAgent: 'RNW Github Release Script',
});

(async () => {
  console.log('Reading changelogs...');
  const changelogs = await readChangelogs();

  console.log('Listing tags...');
  const localTags = (await simplegit().tags()).all;
  console.log(localTags.map(tag => `  - ${tag}`).join('\n'));

  console.log('Fetching releases...');
  const githubReleases = await octokit.paginate(
    'GET /repos/{owner}/{repo}/releases',
    RNW_REPO,
  );

  console.log(
    githubReleases.map(release => `  - ${release.tag_name}`).join('\n'),
  );

  const releasesToPublish: Release[] = [];
  const allReleases: Release[] = [];
  for (const changelog of changelogs) {
    for (const release of aggregateReleases(changelog)) {
      allReleases.push(release);
      if (needsRelease(release, localTags, githubReleases)) {
        releasesToPublish.push(release);
      }
    }
  }

  if (releasesToPublish.length > 1 && !argv.yes) {
    console.error(
      chalk.red(
        'You are about to publish more than a single release. If you have stale' +
          'tags in your local repository (such as if you fetched prior to Jan Feb' +
          '2021) this script can create hundreds of releases at once, each ' +
          'sending an email notification. If you want to proceed, please re-run' +
          'the command with the "--yes" flag.',
      ),
    );
    process.exit(1);
  }

  for (const release of releasesToPublish) {
    await publishRelease(release, allReleases);
  }

  console.log(chalk.green('All done!'));
})();

/**
 * Collects all of the changelogs in the repo
 */
async function readChangelogs(): Promise<Changelog[]> {
  const repoRoot = await findRepoRoot();
  const changelogs: string[] = await glob('**/CHANGELOG.json', {
    cwd: repoRoot,
    ignore: '**/node_modules/**',
  });

  return Promise.all(
    changelogs.map(async changelog => {
      const fullPath = path.join(repoRoot, changelog);
      return JSON.parse((await fs.promises.readFile(fullPath)).toString());
    }),
  );
}

/**
 * Determines whether we should try to make a release for a change entry
 */
function needsRelease(
  release: Release,
  localTags: string[],
  githubReleases: Array<{tag_name: string}>,
) {
  const releaseTags = githubReleases.map(r => r.tag_name);
  return localTags.includes(release.tag) && !releaseTags.includes(release.tag);
}

/**
 * Create a release for the given change entry
 */
async function publishRelease(release: Release, allReleases: Release[]) {
  const pre = semver.prerelease(release.version);
  console.log(
    `Creating release for ${release.packageName} ${release.version}...`,
  );

  await octokit.repos.createRelease({
    tag_name: release.tag,
    name: releaseTitle(release),
    body: releaseMarkdown(release, allReleases),
    prerelease: !!pre,
    draft: shouldBeDraft(release),
    ...RNW_REPO,
  });
}

/**
 * Transforms the changelog JSON into an array of releases.
 */
function aggregateReleases(changelog: Changelog): Release[] {
  const entriesByTag = _.groupBy(changelog.entries, e => e.tag);

  const commentsByTag = _.mapValues(entriesByTag, entries => {
    const comments: Comment[] = [];

    const commentsByType = _.merge({}, ...entries.map(entry => entry.comments));
    const changeTypes = Object.keys(commentsByType).filter(t => t !== 'none');
    changeTypes.forEach(t => comments.push(...commentsByType[t]));
    return comments;
  });

  return Object.keys(commentsByTag).map(tag => ({
    packageName: changelog.name,
    tag,
    version: semver.parse(entriesByTag[tag][0].version)!,
    comments: commentsByTag[tag],
  }));
}

/**
 * Create the markdown representation of a release
 */
function releaseMarkdown(release: Release, allReleases: Release[]): string {
  let md = '';

  if (release.packageName === 'react-native-windows') {
    const mostRecentMajor = mostRecentMajorRelease(release, allReleases);

    if (mostRecentMajor) {
      if (semver.prerelease(release.version)) {
        md +=
          `This is a preview of the next version of react-native-windows. ` +
          `To see a summary of changes in this major release, see [release notes for ${releaseTitle(
            mostRecentMajor,
          )}](${releaseLink(mostRecentMajor)}).\n`;
      } else {
        md +=
          `This is patch release of react-native-windows, fixing bugs or adding non-breaking enhancements. ` +
          `To see a summary of changes in this major release, see [release notes for ${releaseTitle(
            mostRecentMajor,
          )}](${releaseLink(mostRecentMajor)}).\n`;
      }
    } else {
      console.warn(
        chalk.yellow(
          `Could not determine the most recent major version before "${release.version}". Omitting release links`,
        ),
      );
    }
  }

  md += `### New changes\n`;
  for (const change of release.comments) {
    const abbrevCommit = change.commit.substr(0, 8);
    md += `- ${abbrevCommit} ${change.comment} (${change.author})\n`;
  }

  return md;
}

/**
 * The most recently published major (first preview or stable) release. These
 * have curated release notes that we can link to from other releases.
 */
function mostRecentMajorRelease(
  release: Release,
  allReleases: Release[],
): Release | null {
  const releaseSemver = semver.parse(release.version);
  if (!releaseSemver) {
    return null;
  }

  let firstVersion: semver.SemVer;
  if (releaseSemver.prerelease.length === 0) {
    firstVersion = semver.parse(
      `${releaseSemver.major}.${releaseSemver.minor}.0`,
    )!;
  } else if (
    releaseSemver.prerelease.length === 2 &&
    releaseSemver.prerelease[0] === 'preview'
  ) {
    firstVersion = semver.parse(
      `${releaseSemver.major}.${releaseSemver.minor}.${releaseSemver.patch}-${releaseSemver.prerelease[0]}.1`,
    )!;
  } else {
    return null;
  }

  const matchingRelease = allReleases.find(r => {
    return (
      r.packageName === release.packageName &&
      semver.eq(r.version, firstVersion)
    );
  });
  return matchingRelease || null;
}

/**
 * Returns a GitHub release link to a given release
 */
function releaseLink(release: Release): string {
  return `https://github.com/microsoft/react-native-windows/releases/tag/${release.tag}`;
}

/**
 * Return a full release title from a release
 */
function releaseTitle(release: Release): string {
  return `${packageTitle(release.packageName)} ${release.version}`;
}

/**
 * Return a release title name corresponding to a package
 */
function packageTitle(packageName: string): string {
  if (packageName === 'react-native-windows') {
    return 'React Native Windows';
  } else {
    return packageName;
  }
}

/**
 * Should a release be created as a draft?
 */
function shouldBeDraft(release: Release): boolean {
  if (release.packageName !== 'react-native-windows') {
    return false;
  }

  // First preview and first non-prerelease get manually curated notes. Create
  // them as a draft to allow setting notes before the initial email notification.
  const pre = release.version.prerelease;
  const isPrerelease = pre.length !== 0;

  return (
    (isPrerelease && pre[0] === 'preview' && pre[1] === 1) || // 0.x.0-preview.1 is the first preview build
    (!isPrerelease && release.version.patch === 0) // 0.x.0 is the first non-prerelease build
  );
}
