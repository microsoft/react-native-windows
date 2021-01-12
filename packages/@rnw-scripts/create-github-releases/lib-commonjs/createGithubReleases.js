"use strict";
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
Object.defineProperty(exports, "__esModule", { value: true });
const _ = require("lodash");
const chalk = require("chalk");
const fs = require("fs");
const path = require("path");
const semver = require("semver");
const simplegit = require("simple-git/promise");
const util = require("util");
const yargs = require("yargs");
const node_fetch_1 = require("node-fetch");
const find_repo_root_1 = require("@rnw-scripts/find-repo-root");
const glob = util.promisify(require('glob').glob);
const RNW_REPO_ENDPOINT = 'https://api.github.com/repos/microsoft/react-native-windows';
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
async function readChangelogs() {
    const repoRoot = await find_repo_root_1.default();
    const changelogs = await glob('**/CHANGELOG.json', {
        cwd: repoRoot,
        ignore: '**/node_modules/**',
    });
    return Promise.all(changelogs.map(async (changelog) => {
        const fullPath = path.join(repoRoot, changelog);
        return JSON.parse((await fs.promises.readFile(fullPath)).toString());
    }));
}
/**
 * Fetch a list of all releases that Github knows about
 */
async function fetchReleases(token) {
    return await fetchAllPages(`${RNW_REPO_ENDPOINT}/releases`, token);
}
/**
 * Fethches all pages of a given GitHub resource
 *
 * @param baseUrl the resoure URL
 * @param token the GitHub auth token
 * @returns a merged representation of the arrays of all pages
 */
async function fetchAllPages(baseUrl, token) {
    let page = 0;
    const results = [];
    while (true) {
        const pageUrl = new URL(baseUrl);
        pageUrl.searchParams.append('per_page', '100');
        pageUrl.searchParams.append('page', (++page).toString());
        const pageResult = await node_fetch_1.default(pageUrl, {
            headers: requestHeaders(token),
        });
        if (!pageResult.ok) {
            throw new Error(`Status ${pageResult.status}: Unable to fetch '${pageUrl}'`);
        }
        const pageResultArr = await pageResult.json();
        if (pageResultArr.length === 0) {
            return results;
        }
        else {
            results.push(...pageResultArr);
        }
    }
}
/**
 * Determines whether we should try to make a release for a change entry
 */
function needsRelease(release, localTags, githubReleases) {
    const releaseTags = githubReleases.map(r => r.tag_name);
    return localTags.includes(release.tag) && !releaseTags.includes(release.tag);
}
/**
 * Create a release for the given change entry
 */
async function publishRelease(release, token) {
    const pre = semver.prerelease(release.version);
    console.log(`Creating release for ${release.packageName} ${release.version}...`);
    const res = await node_fetch_1.default(`${RNW_REPO_ENDPOINT}/releases`, {
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
function aggregateReleases(changelog) {
    const entriesByTag = _.groupBy(changelog.entries, e => e.tag);
    const commentsByTag = _.mapValues(entriesByTag, entries => {
        const comments = [];
        const commentsByType = _.merge({}, ...entries.map(entry => entry.comments));
        const changeTypes = Object.keys(commentsByType).filter(t => t !== 'none');
        changeTypes.forEach(t => comments.push(...commentsByType[t]));
        return comments;
    });
    return Object.keys(commentsByTag).map(tag => ({
        packageName: changelog.name,
        tag,
        version: semver.parse(entriesByTag[tag][0].version),
        comments: commentsByTag[tag],
    }));
}
/**
 * Create the markdown representation of a release
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
 */
function packageTitle(packageName) {
    if (packageName === 'react-native-windows') {
        return 'React Native Windows';
    }
    else {
        return packageName;
    }
}
/**
 * Should a release be created as a draft?
 */
function shouldBeDraft(release) {
    if (release.packageName !== 'react-native-windows') {
        return false;
    }
    // First preview and first non-prerelease get manually curated notes. Create
    // them as a draft to allow setting notes before the initial email notification.
    const pre = release.version.prerelease;
    const isPrerelease = pre.length !== 0;
    return ((isPrerelease && pre[0] === 'preview' && pre[1] === 1) || // 0.x.0-preview.1 is the first preview build
        (!isPrerelease && release.version.patch === 0) // 0.x.0 is the first non-prerelease build
    );
}
//# sourceMappingURL=createGithubReleases.js.map