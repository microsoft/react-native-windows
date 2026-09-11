// Verdaccio's JavaScript config support is deprecated, so emit YAML instead.
const fs = require('node:fs');
const path = require('node:path');

const FEED_URL =
  'https://pkgs.dev.azure.com/ms/react-native/_packaging/react-native-public/npm/registry/';
const TOKEN_ENV = 'RNW_NPM_FEED_TOKEN';

const lines = [
  'storage: ./storage',
  'auth:',
  '  htpasswd:',
  '    file: ./htpasswd',
  'uplinks:',
  '  npmFeed:',
  `    url: ${FEED_URL}`,
  '    max_fails: 40',
  '    maxage: 30m',
  '    timeout: 60s',
  '    fail_timeout: 10m',
  '    cache: false',
  '    agent_options:',
  '      keepAlive: true',
  '      maxSockets: 40',
  '      maxFreeSockets: 10',
];

// Unauthenticated reads only resolve packages the feed has already cached.
if (process.env[TOKEN_ENV]) {
  lines.push('    auth:', '      type: bearer', `      token_env: ${TOKEN_ENV}`);
}

// Without a feed token the uplink liveness check fails, so publish local packages regardless of uplink state.
lines.push('publish:', '  allow_offline: true');

lines.push(
  'packages:',
  "  '@*/*':",
  '    access: $all',
  '    publish: $all',
  '    proxy: npmFeed',
  "  '**':",
  '    access: $all',
  '    publish: $all',
  '    proxy: npmFeed',
  'logs:',
  '  - {type: file, path: verdaccio.log, format: pretty, level: debug}',
);

const outputPath = path.join(__dirname, 'config.generated.yaml');
fs.writeFileSync(outputPath, `${lines.join('\n')}\n`);

console.log(
  `Wrote ${outputPath} (feed authentication ${process.env[TOKEN_ENV] ? 'enabled' : 'disabled'})`,
);
