/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'fs';
import path from 'path';
import yargs from 'yargs';

const screenshot = require('screenshot-desktop');

const {filename} = yargs.options({
  filename: {
    describe: 'Where to place the screenshot',
    type: 'string',
    demandOption: true,
  },
}).argv;

fs.mkdirSync(path.dirname(filename), {recursive: true});
screenshot({filename});
