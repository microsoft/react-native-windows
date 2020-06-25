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

const _ = require('lodash');
const chalk = require('chalk');
const fetch = require('node-fetch');
const fs = require('fs');
const path = require('path');
const semver = require('semver');
const simplegit = require('simple-git/promise');
const util = require('util');
const yargs = require('yargs');

const glob = util.promisify(require('glob').glob);

const findRepoRoot = require('./findRepoRoot');

const RNW_REPO_ENDPOINT =
  'https://api.github.com/repos/microsoft/react-native-windows';

const argv = yargs
  .options({
    authToken: {
      describe: 'GitHub Auth token',
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
  const releases = await fetchReleases(argv.authToken);

  for (const changelog of changelogs) {
    for (const release of aggregateReleases(changelog)) {
      if (needsRelease(release, localTags, releases)) {
        await publishRelease(release, argv.authToken);
      }
    }
  }

  console.log(chalk.green('All done!'));
})();

/**
 * Collects all of the changelogs in the repo
 *
 * @returns {Array<{name: string, entries: []}>}
 */
async function readChangelogs() {
  const repoRoot = await findRepoRoot();
  const changelogs = await glob('**/CHANGELOG.json', {
    cwd: repoRoot,
    ignore: '**/node_modules/**',
  });

  return Promise.all(
    changelogs.map(async changelog => {
      const fullPath = path.join(repoRoot, changelog);
      return JSON.parse(await fs.promises.readFile(fullPath));
    }),
  );
}

/**
 * Fetch a list of all releases that Github knows about
 *
 * @param {string} token the GitHub auth token
 */
async function fetchReleases(token) {
  return await fetchAllPages(`${RNW_REPO_ENDPOINT}/releases`, token);
}

/**
 * Fethches all pages of a given GitHub resource
 *
 * @param {string} baseUrl the resoure URL
 * @param {string} token the GitHub auth token
 * @returns a merged representation of the arrays of all pages
 */
async function fetchAllPages(baseUrl, token) {
  let page = 0;
  const results = [];

  while (true) {
    const pageUrl = new URL(baseUrl);
    pageUrl.searchParams.append('per_page', 100);
    pageUrl.searchParams.append('page', ++page);

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
 *
 * @param {object} release
 * @param {string[]} localTags
 * @param {object[]} releases
 */
function needsRelease(release, localTags, releases) {
  const releaseTags = releases.map(r => r.tag_name);
  return localTags.includes(release.tag) && !releaseTags.includes(release.tag);
}

/**
 * Create a release for the given change entry
 *
 * @param {string} packageName
 * @param {object} changeEntry
 * @param {string} token the GitHub auth token
 */
async function publishRelease(release, token) {
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
    }),
  });

  if (!res.ok) {
    throw new Error(`Status ${res.status} trying to create release`);
  }
}

/**
 * Transforms the changelog JSON into an array of releases.
 *
 * @param {object} changelog
 */
function aggregateReleases(changelog) {
  const entriesByTag = _.groupBy(changelog.entries, e => e.tag);

  const commentsByTag = _.mapValues(entriesByTag, entries => {
    const comments = [];

    const commentsByType = _.merge(...entries.map(entry => entry.comments));
    const changeTypes = Object.keys(commentsByType).filter(t => t !== 'none');
    changeTypes.forEach(t => comments.push(...commentsByType[t]));
    return comments;
  });

  return Object.keys(commentsByTag).map(tag => ({
    packageName: changelog.name,
    tag: tag,
    version: entriesByTag[tag][0].version,
    comments: commentsByTag[tag],
  }));
}

/**
 * Create the markdown representation of a release
 *
 * @param {object} changeEntry
 */
function createReleaseMarkdown(release) {
  let md = '';

  for (const change of release.comments) {
    const abbrevCommit = change.commit.substr(0, 8);
    md += `- ${abbrevCommit} ${change.comment} (${change.author})\n`;
  }

  return md;
}

/**
 * Create request headers common to GitHub API calls
 *
 * @param {string} token
 */
function requestHeaders(token) {
  return {
    Authorization: `Token ${token}`,
    'Content-Type': 'application/json',
    'User-Agent': 'RNW Github Release Script',
  };
}

/**
 * Return a release title name corresponding to a package
 *
 * @param {string} packageName
 */
function packageTitle(packageName) {
  if (packageName === 'react-native-windows') {
    return 'React Native Windows';
  } else {
    return packageName;
  }
}
