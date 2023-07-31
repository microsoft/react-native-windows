/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const fs = require('fs');
const https = require('https');
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

async function downloadFlowTypes(overwrite = false) {
  const rnDir = path.dirname(require.resolve('react-native/package.json'));
  const reactNativeVersion = require(path.resolve(
    rnDir,
    'package.json',
  )).version;

  const reactNativeRef = await getAbbreviatedRef(reactNativeVersion);

  const typedPath = 'flow-typed/npm';
  const destRootPath = path.resolve(rnDir, '../.flow/');

  // Create target path if necessary
  const destTypedPath = path.resolve(destRootPath, typedPath);
  if (!fs.existsSync(destTypedPath)) {
    fs.mkdirSync(destTypedPath, {recursive: true});
  }

  const octokit = new Octokit({
    userAgent: 'RNW Just Task Script',
  });

  logger.verbose(
    `Getting list of files in react-native@${reactNativeVersion} under /${typedPath}`,
  );

  const ghResponse = await octokit.rest.repos.getContent({
    owner: 'facebook',
    repo: 'react-native',
    ref: reactNativeRef,
    path: typedPath,
  });

  if (ghResponse.status !== 200) {
    throw new Error(
      `Failed to get list of files, Status Code: ${ghResponse.status}.`,
    );
  }

  if (ghResponse.status === 200) {
    for (const fileEntry of ghResponse.data) {
      await downloadFile(
        fileEntry.download_url,
        path.resolve(destRootPath, fileEntry.path),
        overwrite,
      );
    }
  }
}

task('downloadFlowTypes', async () => await downloadFlowTypes(false));

task('downloadFlowTypes:fix', async () => await downloadFlowTypes(true));

task(
  'flow-check',
  series('downloadFlowTypes', async () => {
    require('child_process').execSync('npx flow check', {stdio: 'inherit'});
  }),
);
