"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const ora = require("ora");
const path = require("path");
const semver = require("semver");
const yargs = require("yargs");
const package_utils_1 = require("@react-native-windows/package-utils");
const react_native_platform_override_1 = require("react-native-platform-override");
const runCommand_1 = require("./runCommand");
const upgradeDependencies_1 = require("./upgradeDependencies");
(async () => {
    const { argv } = yargs
        .check(args => {
        if (args._.length === 1 && semver.valid(args._[0])) {
            return true;
        }
        else {
            throw new Error('Usage: integrate-rn <version>');
        }
    })
        .showHelpOnFail(false);
    const version = argv._[0];
    await funcStep(`Updating packages and dependants to react-native@${version}`, async () => await upgradeDependencies_1.default(version));
    await funcStep('Upgrading out-of-date overrides', upgradePlatformOverrides);
    await funcStep('Performing additional override validation', validatePlatformOverrides);
    await failableCommandStep('yarn lint:fix');
    await commandStep('yarn build');
    await failableCommandStep('yarn lint');
})();
/**
 * Enumerate packages subject to override validation
 */
async function enumerateOverridePackages() {
    return await package_utils_1.enumerateRepoPackages(isOverridePackage);
}
/**
 * Whether the NPM package is subject to override validation
 */
async function isOverridePackage(pkg) {
    try {
        await fs.promises.access(path.join(pkg.path, 'overrides.json'));
        return true;
    }
    catch (_a) {
        return false;
    }
}
/**
 * Upgrade platform overrides in the repo to the current version of react
 * native, disallowing files with conflicts to be written
 */
async function upgradePlatformOverrides(spinner) {
    const overridesWithConflicts = [];
    for (const pkg of await enumerateOverridePackages()) {
        const results = await react_native_platform_override_1.upgradeOverrides(path.join(pkg.path, 'overrides.json'), { allowConflicts: false });
        overridesWithConflicts.push(...results
            .filter(r => r.hasConflicts)
            .map(r => r.overrideName)
            .map(o => path.join(pkg.path, o)));
    }
    if (overridesWithConflicts.length !== 0) {
        spinner.warn();
        console.warn(`Conflicts detected in the following files. Use 'react-native-platform-override upgrade' to resolve: \n- ${overridesWithConflicts.join('\n- ')}`);
    }
}
/**
 * Perform additional validation on platform overrides apart from the previous
 * up-to-date check
 */
async function validatePlatformOverrides(spinner) {
    const errors = [];
    for (const pkg of await enumerateOverridePackages()) {
        errors.push(...(await react_native_platform_override_1.validateManifest(path.join(pkg.path, 'overrides.json'))));
    }
    if (errors.filter(e => e.type !== 'outOfDate').length !== 0) {
        spinner.warn();
        console.warn("Additional errors detected. Run 'yarn validate-overrides' for more information");
    }
}
/**
 * Run the function while showing a spinner
 */
async function funcStep(name, func) {
    const spinner = ora(name).start();
    try {
        const t = await func(spinner);
        if (spinner.isSpinning) {
            spinner.succeed();
        }
        return t;
    }
    catch (ex) {
        spinner.fail();
        throw ex;
    }
}
/**
 * Run the command while showing a spinner
 */
async function commandStep(cmd) {
    return funcStep(cmd, async () => runCommand_1.default(cmd));
}
/**
 * Run the command while showing a spinner. Continue if the command returns a non-zero exit code.
 */
async function failableCommandStep(cmd) {
    return funcStep(cmd, async (spinner) => {
        try {
            await runCommand_1.default(cmd);
        }
        catch (ex) {
            spinner.fail();
            console.error(ex.message);
        }
    });
}
//# sourceMappingURL=integrateRN.js.map