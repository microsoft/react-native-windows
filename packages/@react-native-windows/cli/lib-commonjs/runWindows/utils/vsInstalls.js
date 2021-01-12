"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.findLatestVsInstall = exports.enumerateVsInstalls = void 0;
const child_process_1 = require("child_process");
const fs = require("fs");
const path = require("path");
/**
 * Helper to run vswhere in JSON mode
 *
 * @param args Arguments to pass to vsWhere
 * @param verbose enable verbose logging
 */
function vsWhere(args, verbose) {
    // This path is maintained and VS has promised to keep it valid.
    const vsWherePath = path.join(process.env['ProgramFiles(x86)'] || process.env.ProgramFiles, '/Microsoft Visual Studio/Installer/vswhere.exe');
    if (verbose) {
        console.log('Looking for vswhere at: ' + vsWherePath);
    }
    if (!fs.existsSync(vsWherePath)) {
        throw new Error(`Unable to find vswhere at ${vsWherePath}`);
    }
    return JSON.parse(child_process_1.execSync(`"${vsWherePath}" ${args.join(' ')} -format json -utf8`).toString('utf8'));
}
/**
 * Enumerate the installed versions of Visual Studio
 */
function enumerateVsInstalls(opts) {
    const args = [];
    if (opts.version) {
        args.push(`-version [${opts.version},${Number(opts.version) + 1})`);
    }
    if (opts.requires) {
        args.push(`-requires ${opts.requires.join(' ')}`);
    }
    if (opts.latest) {
        args.push('-latest');
    }
    return vsWhere(args, opts.verbose);
}
exports.enumerateVsInstalls = enumerateVsInstalls;
/**
 * Find the latest available VS installation that matches the given constraints
 */
function findLatestVsInstall(opts) {
    const installs = enumerateVsInstalls(Object.assign(Object.assign({}, opts), { latest: true }));
    if (installs.length > 0) {
        return installs[0];
    }
    else {
        return null;
    }
}
exports.findLatestVsInstall = findLatestVsInstall;
//# sourceMappingURL=vsInstalls.js.map