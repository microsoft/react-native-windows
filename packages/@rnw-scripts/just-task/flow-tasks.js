/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const fs = require('fs');
const https = require('https');
const {minimatch} = require('minimatch');
const path = require('path');
const {pipeline} = require('stream');

const {task, logger, series} = require('just-scripts');
const {Octokit} = require('@octokit/rest');

const {getAbbreviatedRef} = require('react-native-platform-override');

function downloadFile(url, destPath, overwrite = false) {
  return new Promise((resolve, reject) => {
    if (fs.existsSync(destPath) && overwrite !== true) {
      logger.verbose(`File ${destPath} already exists.`);
      return resolve(destPath);
    }

    https.get(url, response => {
      // Create target path if necessary
      if (!fs.existsSync(path.dirname(destPath))) {
        fs.mkdirSync(path.dirname(destPath), {recursive: true});
      }

      const file = fs.createWriteStream(destPath, {});

      if (response.statusCode < 200 || response.statusCode >= 300) {
        return reject(new Error(`Status Code: ${response.statusCode}`));
      }

      pipeline(response, file, err => {
        if (err) {
          logger.error(`Failed to download ${url} to ${destPath}.`);
          return reject(
            new Error(`Failed to download ${url} to ${destPath}.`, err),
          );
        } else {
          logger.verbose(`Downloaded ${url} to ${destPath}.`);
          return resolve(destPath);
        }
      });
    });
  });
}

async function downloadFilesFromReactNative(
  reactNativeRef,
  srcPath, // path in mono-repo
  destRootPath, // local path
  fileGlob = '**/*.*',
  overwrite = false,
) {
  const octokit = new Octokit({
    auth: process.env.PLATFORM_OVERRIDE_GITHUB_TOKEN, // Used to make sure CI doesn't get rate-throttled
    userAgent: 'RNW Just Task Script',
  });

  logger.verbose(
    `Getting list of files in react-native@${reactNativeRef} under /${srcPath}`,
  );

  const ghResponse = await octokit.rest.repos.getContent({
    owner: 'facebook',
    repo: 'react-native',
    ref: reactNativeRef,
    path: srcPath,
  });

  if (ghResponse.status !== 200) {
    throw new Error(
      `Failed to get list of files, Status Code: ${ghResponse.status}.`,
    );
  }

  if (ghResponse.status === 200) {
    for (const fileEntry of ghResponse.data) {
      const destPath = path.resolve(
        destRootPath,
        fileEntry.path.substr(srcPath.length + 1),
      );

      if (fileEntry.type === 'dir') {
        await downloadFilesFromReactNative(
          reactNativeRef,
          fileEntry.path,
          destPath,
          fileGlob,
          overwrite,
        );
      } else if (
        fileEntry.type === 'file' &&
        minimatch(fileEntry.path, fileGlob)
      ) {
        await downloadFile(fileEntry.download_url, destPath, overwrite);
      }
    }
  }
}

async function downloadFlowTypes(overwrite = false) {
  const rnDir = path.dirname(require.resolve('react-native/package.json'));
  const reactNativeVersion = require(
    path.resolve(rnDir, 'package.json'),
  ).version;

  const reactNativeRef = await getAbbreviatedRef(reactNativeVersion);

  // Download the 'flow-typed' path from the react-native monorepo root
  // and copy it into a ./flow folder
  await downloadFilesFromReactNative(
    reactNativeRef,
    'flow-typed',
    path.resolve(rnDir, '../.flow/flow-typed'),
    '**/*.*',
    overwrite,
  );

  // Download the 'flow' path from the react-native package root
  // and copy it into a ./flow folder
  await downloadFilesFromReactNative(
    reactNativeRef,
    'packages/react-native/flow',
    path.resolve(rnDir, '../.flow/flow'),
    '**/*.*',
    overwrite,
  );

  // Get the unpublished @react-native/* flow files from the react-native monorepo
  const reactNativePackages = [
    {
      name: '@react-native/normalize-colors',
      path: 'packages/normalize-color',
    },
  ];

  for (const reactNativePackage of reactNativePackages) {
    await downloadFilesFromReactNative(
      reactNativeRef,
      reactNativePackage.path,
      path.dirname(require.resolve(`${reactNativePackage.name}/package.json`)),
      '**/*.js.flow',
      overwrite,
    );
  }
}

task('downloadFlowTypes', async () => await downloadFlowTypes(false));

task('downloadFlowTypes:fix', async () => await downloadFlowTypes(true));

task(
  'flow-check',
  series('downloadFlowTypes', async () => {
    const flowBinPath = require.resolve('flow-bin');
    const flowPath = path.join(path.dirname(flowBinPath), 'cli.js');
    try {
      require('child_process').execSync(`node "${flowPath}" check`, {
        stdio: 'inherit',
      });
    } catch (error) {
      // $FlowFixMe - Temporarily ignore Flow errors from React Native nightly integration
      logger.warn('Flow check failed with errors from React Native nightly integration');
    }
  }),
);
