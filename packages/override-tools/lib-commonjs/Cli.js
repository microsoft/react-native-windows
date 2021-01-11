"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
const FileSearch = require("./FileSearch");
const ManifestData = require("./ManifestData");
const OverridePrompt = require("./OverridePrompt");
const chalk = require("chalk");
const fs = require("fs");
const ora = require("ora");
const path = require("path");
const yargs = require("yargs");
const Manifest_1 = require("./Manifest");
const FileRepository_1 = require("./FileRepository");
const OverrideUpgrader_1 = require("./OverrideUpgrader");
const CrossProcessLock_1 = require("./CrossProcessLock");
const GitReactFileRepository_1 = require("./GitReactFileRepository");
const OverrideFileRepositoryImpl_1 = require("./OverrideFileRepositoryImpl");
const ReactVersion_1 = require("./ReactVersion");
doMain(() => {
    return new Promise((resolve, _) => {
        yargs
            .command('validate <manifests...>', 'Verify that overrides are recorded and up-to-date', cmdYargs => cmdYargs.options({
            manifests: {
                type: 'array',
                describe: 'Paths to the override manifests to validate',
            },
            version: {
                type: 'string',
                describe: 'Optional React Native version to check against',
            },
        }), cmdArgv => validateManifests(cmdArgv.manifests, cmdArgv.version))
            .command('add <override>', 'Add an override to the manifest', cmdYargs => cmdYargs.options({
            override: { type: 'string', describe: 'The override to add' },
        }), cmdArgv => addOverride(cmdArgv.override))
            .command('remove <override>', 'Remove an override from the manifest', cmdYargs => cmdYargs.options({
            override: { type: 'string', describe: 'The override to remove' },
        }), cmdArgv => removeOverride(cmdArgv.override))
            .command('auto-upgrade <manifest>', 'Attempts to automatically merge new changes into out-of-date overrides', cmdYargs => cmdYargs.options({
            manifest: {
                type: 'string',
                describe: 'Path to an override manifest',
            },
            version: {
                type: 'string',
                describe: 'Optional React Native version to check against',
            },
        }), cmdArgv => autoUpgrade(cmdArgv.manifest, cmdArgv.version))
            .command('manual-upgrade <manifest>', 'Similar to auto-upgrade, but places conflict markers in files that cannot be automatically merged', cmdYargs => cmdYargs.options({
            manifest: {
                type: 'string',
                describe: 'Path to an override manifest',
            },
            version: {
                type: 'string',
                describe: 'Optional React Native version to check against',
            },
        }), cmdArgv => manualUpgrade(cmdArgv.manifest, cmdArgv.version))
            .epilogue('This tool allows managing JavaScript overrides for React Native Windows')
            .option('color', { hidden: true })
            .demandCommand()
            .recommendCommands()
            .strict()
            .showHelpOnFail(false)
            .wrap(yargs.terminalWidth())
            .version(false)
            .scriptName('override')
            .onFinishCommand(resolve).argv;
    });
});
/**
 * Check that the given manifests correctly describe overrides and that all
 * overrides are up to date
 */
function validateManifests(manifestPaths, version) {
    return __awaiter(this, void 0, void 0, function* () {
        for (const manifestPath of manifestPaths) {
            if (typeof manifestPath !== 'string') {
                throw new Error('manifest arguments must be strings');
            }
            yield validateManifest(manifestPath, version);
        }
    });
}
function validateManifest(manifestPath, version) {
    return __awaiter(this, void 0, void 0, function* () {
        const fullPath = path.resolve(manifestPath);
        const spinner = ora(`Verifying overrides in ${fullPath}`).start();
        yield spinnerGuard(spinner, () => __awaiter(this, void 0, void 0, function* () {
            const manifest = yield readManifest(manifestPath, version);
            const validationErrors = yield manifest.validate();
            if (validationErrors.length === 0) {
                spinner.succeed();
            }
            else {
                spinner.fail();
                printValidationErrors(validationErrors);
                process.exitCode = 1;
            }
        }));
    });
}
/**
 * Add an override to the manifest
 */
function addOverride(overridePath) {
    return __awaiter(this, void 0, void 0, function* () {
        yield checkFileExists('override', overridePath);
        const manifestPath = yield FileSearch.findManifest(overridePath);
        const manifestDir = path.dirname(manifestPath);
        const manifest = yield readManifest(manifestPath);
        const relOverride = path.normalize(path.relative(manifestDir, overridePath));
        if (manifest.hasOverride(relOverride)) {
            console.warn(chalk.yellow('Warning: override already exists in manifest and will be overwritten'));
            manifest.removeOverride(relOverride);
        }
        const overrideDetails = yield OverridePrompt.askForDetails(relOverride);
        const spinner = ora('Adding override (This may take a while on first run)').start();
        yield spinnerGuard(spinner, () => __awaiter(this, void 0, void 0, function* () {
            yield manifest.addOverride(overrideDetails.type, relOverride, overrideDetails.baseFile, overrideDetails.issue);
            const manifestData = manifest.getAsData();
            yield ManifestData.writeToFile(manifestData, manifestPath);
            spinner.succeed('Adding override');
        }));
    });
}
/**
 * Remove an override from the manifest
 */
function removeOverride(overridePath) {
    return __awaiter(this, void 0, void 0, function* () {
        const manifestPath = yield FileSearch.findManifest(overridePath);
        const manifestDir = path.dirname(manifestPath);
        const manifest = yield readManifest(manifestPath);
        const relOverride = path.normalize(path.relative(manifestDir, overridePath));
        if (!manifest.removeOverride(relOverride)) {
            console.error(chalk.red('Could not remove override. Is it part of the manifest?'));
            process.exit(1);
        }
        yield ManifestData.writeToFile(manifest.getAsData(), manifestPath);
        console.log(chalk.greenBright('Override successfully removed'));
    });
}
/**
 * Attempts to automatically merge changes from the current version into
 * out-of-date overrides.
 */
function autoUpgrade(manifestPath, version) {
    return __awaiter(this, void 0, void 0, function* () {
        return doUpgrade(manifestPath, false /*isManual*/, version);
    });
}
/**
 * Similar to auto-upgrade, but places conflict markers in files that cannot be
 * automatically merged.
 */
function manualUpgrade(manifestPath, version) {
    return __awaiter(this, void 0, void 0, function* () {
        return doUpgrade(manifestPath, true /*isManual*/, version);
    });
}
/**
 * Helper for autoUpgrade and manualUpgrade
 */
function doUpgrade(manifestPath, isManual, version) {
    return __awaiter(this, void 0, void 0, function* () {
        const spinner = ora('Merging overrides').start();
        yield spinnerGuard(spinner, () => __awaiter(this, void 0, void 0, function* () {
            const reactRepo = yield GitReactFileRepository_1.default.createAndInit();
            const ovrRepo = new OverrideFileRepositoryImpl_1.default(path.dirname(manifestPath));
            const upgrader = new OverrideUpgrader_1.default(reactRepo, ovrRepo);
            const manifest = yield readManifestUsingRepos(manifestPath, ovrRepo, reactRepo, version);
            const outOfDateOverrides = (yield manifest.validate())
                .filter(err => err.type === 'outOfDate')
                .map(err => err.file)
                .map(file => manifest.findOverride(file));
            const upgradedOverrides = [];
            let i = 0;
            for (const override of outOfDateOverrides) {
                spinner.text = `Merging overrides (${++i}/${outOfDateOverrides.length})`;
                const upgraded = yield upgrader.getUpgraded(override, manifest.currentVersion());
                if (isManual || !upgraded.hasConflicts) {
                    yield ovrRepo.setFileContents(upgraded.overrideFile, upgraded.content);
                    yield manifest.markUpToDate(upgraded.overrideFile);
                }
                upgradedOverrides.push(upgraded);
            }
            if (upgradedOverrides.length > 0) {
                yield ManifestData.writeToFile(manifest.getAsData(), manifestPath);
            }
            spinner.succeed();
            printUpgradeStats(upgradedOverrides, isManual);
        }));
    });
}
/**
 * Print statistics about an attempt to upgrade out-of-date-overrides.
 */
function printUpgradeStats(results, isManual) {
    const numTotal = results.length;
    const numConflicts = results.filter(res => res.hasConflicts).length;
    const numAutoPatched = numTotal - numConflicts;
    if (numTotal === 0) {
        console.log(chalk.greenBright('No out-of-date overrides detected'));
    }
    else {
        console.log(chalk.greenBright(`${numAutoPatched}/${numTotal} out-of-date overrides automatically merged`));
    }
    if (isManual && numConflicts > 0) {
        console.log(chalk.yellowBright(`${numConflicts} overrides require manual resolution`));
    }
}
/**
 * Prints validation errors in a user-readable form to stderr
 */
function printValidationErrors(errors) {
    if (errors.length === 0) {
        return;
    }
    // Add an initial line of separation
    console.error();
    errors.sort((a, b) => a.file.localeCompare(b.file));
    const filesMissing = errors.filter(err => {
        return err.type === 'fileMissingFromManifest';
    });
    const overridesMissing = errors.filter(err => {
        return err.type === 'overrideFileNotFound';
    });
    const baseFilesNotFound = errors.filter(err => {
        return err.type === 'baseFileNotFound';
    });
    const outOfDateFiles = errors.filter(err => {
        return err.type === 'outOfDate';
    });
    if (filesMissing.length > 0) {
        const errorMessage = "Found override files that aren't listed in the manifest. Overrides can be added to the manifest by using 'yarn override add <override>' (where override is package relative):";
        console.error(chalk.red(errorMessage));
        filesMissing.forEach(err => console.error(` - ${err.file}`));
        console.error();
    }
    if (overridesMissing.length > 0) {
        const errorMessage = "Found overrides in the manifest that don't exist on disk. Remove existing overrides using 'yarn override remove <override>' (where override is package relative):";
        console.error(chalk.red(errorMessage));
        overridesMissing.forEach(err => console.error(` - ${err.file}`));
        console.error();
    }
    if (baseFilesNotFound.length > 0) {
        const errorMessage = "Found overrides whose original files do not exist. Remove existing overrides using 'yarn override remove <override>' (where override is package relative):";
        console.error(chalk.red(errorMessage));
        baseFilesNotFound.forEach(err => console.error(` - ${err.file}`));
        console.error();
    }
    if (outOfDateFiles.length > 0) {
        const errorMessage = "Found overrides whose original files have changed. Upgrade overrides using 'yarn override auto-upgrade <manifest>' and 'yarn override manual-upgrade <manifest>' (where manifest is package relative):";
        console.error(chalk.red(errorMessage));
        outOfDateFiles.forEach(err => console.error(` - ${err.file}`));
        console.error();
    }
}
/**
 * Throw if a file doesn't exist, printing an error message on the way
 */
function checkFileExists(friendlyName, filePath) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            yield fs.promises.access(filePath);
        }
        catch (ex) {
            throw new Error(`Could not find ${friendlyName} at path '${filePath}'`);
        }
    });
}
/**
 * Read a manifest and print a pretty error if we can't
 */
function readManifest(file, version) {
    return __awaiter(this, void 0, void 0, function* () {
        const gitReactRepo = yield GitReactFileRepository_1.default.createAndInit();
        const ovrRepo = new OverrideFileRepositoryImpl_1.default(path.dirname(file));
        return readManifestUsingRepos(file, ovrRepo, gitReactRepo, version);
    });
}
/**
 * Like {@see readManifest}, but allows providing the file repositories to use.
 */
function readManifestUsingRepos(file, ovrRepo, versionedReactRepo, version) {
    return __awaiter(this, void 0, void 0, function* () {
        yield checkFileExists('manifest', file);
        let data;
        try {
            data = yield ManifestData.readFromFile(file);
        }
        catch (ex) {
            throw new Error('Could not parse manifest. Is it valid?');
        }
        const rnVersion = version || (yield ReactVersion_1.getInstalledRNVersion(file));
        const reactRepo = FileRepository_1.bindVersion(versionedReactRepo, rnVersion);
        return new Manifest_1.default(data, ovrRepo, reactRepo);
    });
}
/**
 * Wraps the function in a try/catch, failing the spinner if an exception is
 * thrown to allow unmangled output
 */
function spinnerGuard(spinner, fn) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            return yield fn();
        }
        catch (ex) {
            if (spinner.isSpinning) {
                spinner.fail();
            }
            throw ex;
        }
    });
}
/**
 * Wrap the main function around a barrier to ensure only one copy of the
 * override tool is running at once. This is needed to avoid multiple tools
 * accessing the same local Git repo at the same time.
 */
function doMain(fn) {
    return __awaiter(this, void 0, void 0, function* () {
        const lock = new CrossProcessLock_1.default('react-native-windows-override-cli-lock');
        if (!(yield lock.tryLock())) {
            const spinner = ora('Waiting for other instances of the override CLI to finish').start();
            yield lock.lock();
            spinner.stop();
        }
        yield fn();
        lock.unlock();
    });
}
//# sourceMappingURL=Cli.js.map