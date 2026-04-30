/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Wrapper script for fork-sync that points to the react-native-windows root.
 *
 * Usage:
 *   node packages/@rnw-scripts/fork-sync/lib-commonjs/sync.js --dep fmt
 *   node packages/@rnw-scripts/fork-sync/lib-commonjs/sync.js --dep fmt --status
 *   node packages/@rnw-scripts/fork-sync/lib-commonjs/sync.js --dep fmt --continue
 *
 * All arguments are forwarded to fork-sync with `-C` prepended
 * so it finds the sync-manifest.json in the repository root.
 *
 * @format
 */

import {execFileSync} from 'child_process';
import * as path from 'path';

const syncScript = path.join(
  __dirname,
  '..',
  'node_modules',
  '@rnx-kit',
  'fork-sync',
  'lib',
  'sync.js',
);

const repoRoot = path.join(__dirname, '..', '..', '..', '..');
const args = [syncScript, '-C', repoRoot, ...process.argv.slice(2)];

try {
  execFileSync('node', args, {stdio: 'inherit'});
} catch (e: unknown) {
  // execFileSync throws on non-zero exit; stdio is already inherited
  const err = e as {status?: number};
  process.exit(err.status ?? 1);
}
