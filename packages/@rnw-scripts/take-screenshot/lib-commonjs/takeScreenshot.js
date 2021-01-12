"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const path = require("path");
const yargs = require("yargs");
const screenshot = require('screenshot-desktop');
const { filename } = yargs.options({
    filename: {
        describe: 'Where to place the screenshot',
        type: 'string',
        demandOption: true,
    },
}).argv;
fs.mkdirSync(path.dirname(filename), { recursive: true });
screenshot({ filename });
//# sourceMappingURL=takeScreenshot.js.map