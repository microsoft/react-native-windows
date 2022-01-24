/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fetch from 'node-fetch';
import semver from 'semver';

export function getAbbreviatedRef(reactNativeVersion: string): string {
  if (!semver.valid(reactNativeVersion)) {
    throw new Error(`${reactNativeVersion} is not a valid semver version`);
  }

  if (semver.lt(reactNativeVersion, '0.0.0', {includePrerelease: true})) {
    return extractHashFromNightlyVersion(reactNativeVersion);
  } else {
    return `v${reactNativeVersion}`;
  }
}

export async function fetchFullRef(
  reactNativeVersion: string,
  opts?: {githubToken?: string},
): Promise<string> {
  if (!semver.valid(reactNativeVersion)) {
    throw new Error(`${reactNativeVersion} is not a valid semver version`);
  }

  if (semver.lt(reactNativeVersion, '0.0.0', {includePrerelease: true})) {
    const abbrevHash = extractHashFromNightlyVersion(reactNativeVersion);
    return fetchFullCommitHash(abbrevHash, opts);
  } else {
    return `refs/tags/v${reactNativeVersion}`;
  }
}

// Nightly build have progressed from
// 1. 0.0.0-<commitHash>
// 2. 0.0.0-<commitHash>-<date>-<time> (transitional)
// 3. 0.0.0-<date>-<time>-<commitHash>
function extractHashFromNightlyVersion(reactNativeVersion: string): string {
  // Several versions do not fall into the normal rule for positioning. Special
  // case them.
  const transitionalVersions = [
    '0.0.0-2d2de744b-20210929-2246',
    '0.0.0-1ee16fc2f-20210930-2008',
    '0.0.0-34b7d22f3-20211001-2008',
    '0.0.0-36f3bf2f5-20211002-2010',
    '0.0.0-36f3bf2f5-20211003-2009',
    '0.0.0-36f3bf2f5-20211004-2008',
    '0.0.0-046b02628-20211005-2008',
    '0.0.0-22801870f-20211006-2008',
    '0.0.0-e612d3a11-20211007-2009',
    '0.0.0-c901c43d1-20211008-2008',
    '0.0.0-232d02c5f-20211009-2008',
    '0.0.0-232d02c5f-20211009-2008',
    '0.0.0-232d02c5f-20211009-2008',
    '0.0.0-232d02c5f-20211009-2008',
    '0.0.0-91adb761c-20211013-2010',
  ];

  const preSegment = semver.prerelease(reactNativeVersion)![0];
  const splitPre = preSegment.split('-');

  // Handle #1
  if (splitPre.length === 1) {
    return splitPre[0];
  }

  // Handle #2
  if (transitionalVersions.includes(reactNativeVersion)) {
    return splitPre[0];
  }

  // Handle #3
  return splitPre[2];
}

async function fetchFullCommitHash(
  abbrevHash: string,
  opts?: {githubToken?: string},
): Promise<string> {
  // We cannot get abbreviated hash directly from a remote, so query Github's
  // API for it.
  const commitInfo = await fetch(
    `https://api.github.com/repos/facebook/react-native/commits/${abbrevHash}`,
    {
      headers: {
        'Content-Type': 'application/json',
        'User-Agent': 'react-native-platform-override',
        ...(opts?.githubToken && {Authorization: `Token ${opts.githubToken}`}),
      },
    },
  );
  if (!commitInfo.ok) {
    throw new Error(
      `Unable to query Github for commit '${abbrevHash}' Status: [${commitInfo.status}] '${commitInfo.statusText}'`,
    );
  }

  return (await commitInfo.json()).sha;
}
