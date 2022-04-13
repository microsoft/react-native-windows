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

import * as _ from 'lodash';
import * as chalk from 'chalk';
import * as fs from 'fs';
import * as path from 'path';
import * as semver from 'semver';
import simplegit from 'simple-git';
import * as util from 'util';
import * as yargs from 'yargs';

import fetch from 'node-fetch';
import findRepoRoot from '@react-native-windows/find-repo-root';

const glob = util.promisify(require('glob').glob);

const RNW_REPO_ENDPOINT =
  'https://api.github.com/repos/microsoft/react-native-windows';

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

/**
 * Some of the props exposed on a release by GitHub's REST API for a release
 * resource
 */
interface GithubApiRelease {
  url: string;
  id: number;
  tag_name: string;
  name: string;
  body: string;
  draft: boolean;
  prerelease: boolean;
  created_at: string;
  published_at: boolean;
}

const argv = yargs
  .options({
    authToken: {
      describe: 'GitHub Auth token',
      type: 'string',
      demandOption: true,
    },
  })
  .version(false).argv;

(async () => {
  console.log('Reading changelogs...');
  const changelogs = await readChangelogs();

  console.log('Listing tags...');
  const localTags = (await simplegit().tags()).all;

  console.log('Fetching releases...');
  const githubReleases = await fetchReleases(argv.authToken);

  for (const changelog of changelogs) {
    for (const release of aggregateReleases(changelog)) {
      if (needsRelease(release, localTags, githubReleases)) {
        await publishRelease(release, argv.authToken);
      }
    }
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
 * Fetch a list of all releases that Github knows about
 */
async function fetchReleases(token: string): Promise<GithubApiRelease[]> {
  return await fetchAllPages(`${RNW_REPO_ENDPOINT}/releases`, token);
}

/**
 * Fethches all pages of a given GitHub resource
 *
 * @param baseUrl the resoure URL
 * @param token the GitHub auth token
 * @returns a merged representation of the arrays of all pages
 */
async function fetchAllPages(baseUrl: string, token: string): Promise<any[]> {
  let page = 0;
  const results = [];

  while (true) {
    const pageUrl = new URL(baseUrl);
    pageUrl.searchParams.append('per_page', '100');
    pageUrl.searchParams.append('page', (++page).toString());

    const pageResult = await fetch(pageUrl, {
      headers: requestHeaders(token),
    });

    if (!pageResult.ok) {
      throw new Error(
        `Status ${pageResult.status}: Unable to fetch '${pageUrl}'`,
      );
    }

    const pageResultArr = await pageResult.json();
    if (pageResultArr.length === 0) {
      return results;
    } else {
      results.push(...pageResultArr);
    }
  }
}

/**
 * Determines whether we should try to make a release for a change entry
 */
function needsRelease(
  release: Release,
  localTags: string[],
  githubReleases: GithubApiRelease[],
) {
  const releaseTags = githubReleases.map(r => r.tag_name);
  return localTags.includes(release.tag) && !releaseTags.includes(release.tag);
}

/**
 * Create a release for the given change entry
 */
async function publishRelease(release: Release, token: string) {
  const pre = semver.prerelease(release.version);
  console.log(
    `Creating release for ${release.packageName} ${release.version}...`,
  );

  const res = await fetch(`${RNW_REPO_ENDPOINT}/releases`, {
    method: 'POST',
    headers: requestHeaders(token),
    body: JSON.stringify({
      tag_name: release.tag,
      name: `${packageTitle(release.packageName)} ${release.version}`,
      body: createReleaseMarkdown(release),
      prerelease: !!pre,
      draft: shouldBeDraft(release),
    }),
  });

  if (!res.ok) {
    throw new Error(`Status ${res.status} trying to create release`);
  }
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
function createReleaseMarkdown(release: Release): string {
  let md = '';

  for (const change of release.comments) {
    const abbrevCommit = change.commit.substr(0, 8);
    md += `- ${abbrevCommit} ${change.comment} (${change.author})\n`;
  }

  return md;
}

/**
 * Create request headers common to GitHub API calls
 */
function requestHeaders(token: string): {[header: string]: string} {
  return {
    Authorization: `Token ${token}`,
    'Content-Type': 'application/json',
    'User-Agent': 'RNW Github Release Script',
  };
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
