/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Warm an Azure DevOps upstream feed with the latest patch of every npm/NuGet
 * major.minor line already in the feed, so network-isolated PR/CI builds can
 * restore them anonymously. Runs locally (az login / PAT) and in a pipeline (MI).
 *
 * @format
 */

import {parseArgs} from 'node:util';
import path from 'node:path';
import type {RunOptions} from './types';
import {run} from './run';

const HELP = `warm-feed — warm an Azure DevOps upstream feed by enumerating the
feed and re-pulling the latest patch of each major.minor line already in use.

Usage: npx warm-feed [options]

Options:
  -c, --config <path>     Config file (default: ../warm-feed.config.json)
      --only <npm|nuget>  Restrict to one ecosystem
      --packages <spec>   One-off warm 'eco:id@version' (repeatable / space-list);
                          skips enumeration and warms exactly these versions
      --closure <spec>    Warm the full npm graph of 'npm:id@version' (repeatable /
                          space-list); resolves via npm --package-lock-only
      --closure-manifest <path>
                          Warm the full npm graph of a package.json's deps (repeatable)
      --closure-module <name>
                          Run a special closure module by name, or 'all' for every
                          enabled one (repeatable). E.g. create-react-native-library
      --repo-root <path>  Repo root for modules that read repo files (default: cwd)
      --dry-run           Enumerate and plan only; do not warm
      --verify            Warm every target, ignoring the already-cached skip
      --concurrency <n>   Parallel requests (default from config)
      --pat <token>       ADO PAT (else $env:ADO_PAT / $env:WARM_FEED_TOKEN / az)
  -v, --verbose           Verbose logging
  -h, --help              Show this help
`;

function defaultConfigPath(): string {
  return path.resolve(__dirname, '..', 'warm-feed.config.json');
}

const {values} = parseArgs({
  options: {
    config: {type: 'string', short: 'c'},
    only: {type: 'string'},
    packages: {type: 'string', multiple: true},
    closure: {type: 'string', multiple: true},
    'closure-manifest': {type: 'string', multiple: true},
    'closure-module': {type: 'string', multiple: true},
    'repo-root': {type: 'string'},
    'dry-run': {type: 'boolean'},
    verify: {type: 'boolean'},
    concurrency: {type: 'string'},
    pat: {type: 'string'},
    verbose: {type: 'boolean', short: 'v'},
    help: {type: 'boolean', short: 'h'},
  },
  allowPositionals: false,
});

if (values.help) {
  process.stdout.write(HELP);
  process.exit(0);
}

const options: RunOptions = {
  configPath: values.config ?? defaultConfigPath(),
  only: values.only,
  dryRun: Boolean(values['dry-run']),
  verify: Boolean(values.verify),
  concurrency: values.concurrency ? Number(values.concurrency) : undefined,
  packages: values.packages ?? [],
  closureRoots: values.closure ?? [],
  closureManifests: values['closure-manifest'] ?? [],
  closureModules: values['closure-module'] ?? [],
  repoRoot: values['repo-root'],
  verbose: Boolean(values.verbose),
};

run(options, values.pat)
  .then(code => process.exit(code))
  .catch(err => {
    process.stderr.write(
      `${err instanceof Error ? (err.stack ?? err.message) : String(err)}\n`,
    );
    process.exit(1);
  });
