#!/usr/bin/env node

require('ts-node').register({transpileOnly: true, dir: __dirname});
require('src/Cli.ts');