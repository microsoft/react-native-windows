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
export {};
