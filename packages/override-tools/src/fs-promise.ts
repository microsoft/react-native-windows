/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as util from 'util';

export const exists = util.promisify(fs.exists);
export const mkdir = util.promisify(fs.mkdir);
export const readdir = util.promisify(fs.readdir);
export const readFile = util.promisify(fs.readFile);
export const stat = util.promisify(fs.stat);
