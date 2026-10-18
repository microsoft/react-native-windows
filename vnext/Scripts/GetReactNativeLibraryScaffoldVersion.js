/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

const {spawnSync} = require('node:child_process');

// cRNL 0.63.0's SUPPORTED_REACT_NATIVE_VERSION, not a registry dist-tag.
const createLibraryVersion = '0.63.0';
const scaffoldVersion = '0.85.0';

function checkVersion(name) {
  const spec = `${name}@${scaffoldVersion}`;
  const result = spawnSync('npm', ['view', spec, 'version', '--json'], {
    encoding: 'utf8',
    shell: process.platform === 'win32',
  });
  if (result.error) {
    throw new Error(`Unable to resolve ${spec}: ${result.error.message}`);
  }
  if (result.status !== 0) {
    throw new Error(
      `Unable to resolve ${spec} (exit ${result.status}): ${result.stderr.trim()}`,
    );
  }

  let version;
  try {
    version = JSON.parse(result.stdout);
  } catch (error) {
    throw new Error(
      `Invalid registry version response for ${spec}: ${error.message}`,
    );
  }
  if (version !== scaffoldVersion) {
    throw new Error(
      `Expected ${spec}, but the registry returned ${JSON.stringify(version)}`,
    );
  }
}

try {
  if (process.argv.length !== 3 || process.argv[2] !== createLibraryVersion) {
    throw new Error(
      `No nightly scaffold is configured for create-react-native-library@${process.argv[2]}. ` +
        `Update the bootstrap version to the generator's supported React Native version when changing cRNL.`,
    );
  }
  checkVersion('react-native');
  checkVersion('@react-native-community/template');
  process.stdout.write(`${scaffoldVersion}\n`);
} catch (error) {
  console.error(`GetReactNativeLibraryScaffoldVersion: ${error.message}`);
  process.exitCode = 1;
}
