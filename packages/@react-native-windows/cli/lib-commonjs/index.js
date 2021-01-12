"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.projectConfig = exports.dependencyConfig = exports.commands = exports.generateWindows = void 0;
const fs = require("fs");
const path = require("path");
const telemetry_1 = require("@react-native-windows/telemetry");
const generator_windows_1 = require("./generator-windows");
const autolink_1 = require("./runWindows/utils/autolink");
const runWindows_1 = require("./runWindows/runWindows");
const dependencyConfig_1 = require("./config/dependencyConfig");
const projectConfig_1 = require("./config/projectConfig");
function scrubOptions(opt) {
    return {
        overwrite: opt.overwrite,
        language: opt.language,
        experimentalNuGetDependency: opt.experimentalNuGetDependency,
        nuGetTestFeed: opt.nuGetTestFeed !== undefined ? true : false,
        nuGetTestVersion: opt.nuGetTestVersion !== undefined ? true : false,
        useWinUI3: opt.useWinUI3,
        verbose: opt.verbose,
    };
}
/**
 * Simple utility for running the Windows generator.
 *
 * @param  projectDir root project directory (i.e. contains index.js)
 * @param  name       name of the root JS module for this app
 * @param  ns         namespace for the project
 * @param  options    command line options container
 */
async function generateWindows(projectDir, name, ns, options) {
    var _a, _b, _c;
    let error;
    try {
        if (!fs.existsSync(projectDir)) {
            fs.mkdirSync(projectDir);
        }
        generator_windows_1.installDependencies(options);
        const templateRoot = path.join(__dirname, '..', 'templates');
        await generator_windows_1.copyProjectTemplateAndReplace(templateRoot, projectDir, name, ns, options);
    }
    catch (e) {
        error = e;
        (_a = telemetry_1.Telemetry.client) === null || _a === void 0 ? void 0 : _a.trackException({ exception: error });
        throw e;
    }
    finally {
        if (telemetry_1.Telemetry.client) {
            let rnVersion = '';
            let cliVersion = '';
            try {
                const cwd = process.cwd();
                const rnwPkg = JSON.parse(fs
                    .readFileSync(require.resolve('react-native-windows/package.json', {
                    paths: [cwd],
                }))
                    .toString());
                rnVersion = rnwPkg.peerDependencies['react-native'] || '';
                const rnwCliPkgJson = require('../package.json');
                cliVersion = rnwCliPkgJson.version;
            }
            catch (_d) { }
            const optScrubbed = scrubOptions(options);
            (_b = telemetry_1.Telemetry.client) === null || _b === void 0 ? void 0 : _b.trackEvent({
                name: 'generate-windows',
                properties: Object.assign(Object.assign({ error: error }, optScrubbed), { 'react-native': rnVersion, 'cli-version': cliVersion }),
            });
            (_c = telemetry_1.Telemetry.client) === null || _c === void 0 ? void 0 : _c.flush();
        }
    }
}
exports.generateWindows = generateWindows;
// Assert the interface here doesn't change for the reasons above
const assertStableInterface = true;
assertStableInterface;
exports.commands = [autolink_1.autoLinkCommand, runWindows_1.runWindowsCommand];
exports.dependencyConfig = dependencyConfig_1.dependencyConfigWindows;
exports.projectConfig = projectConfig_1.projectConfigWindows;
//# sourceMappingURL=index.js.map