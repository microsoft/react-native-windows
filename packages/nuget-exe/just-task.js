/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const fs = require('fs');
const https = require('https');
const {argv, task, cleanTask, logger, option} = require('just-scripts');

const NUGET_VERSION = '5.8.0';
const nugetBinary = `./nuget-${NUGET_VERSION}.exe`;
const nugetUrl = `https://dist.nuget.org/win-x86-commandline/v${NUGET_VERSION}/nuget.exe`;

option('clean');

task('clean', cleanTask(nugetBinary));

task('build', async () => {
  if (argv.clean || !fs.existsSync(nugetBinary)) {
    logger.info(`Downloading NuGet binary from "${nugetUrl}"`);
    await downloadFile(nugetUrl, nugetBinary);
  }
});

function downloadFile(url, dest) {
  const destFile = fs.createWriteStream(dest);

  return new Promise((resolve, reject) => {
    https
      .get(url)
      .on('response', res => res.pipe(destFile))
      .on('finish', () => {
        destFile.on('finish', () => {
          destFile.close();
          resolve();
        });
      })
      .on('error', err => {
        fs.unlink(dest, () => reject(err));
      });
  });
}
