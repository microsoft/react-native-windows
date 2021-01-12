"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.copyAndReplaceAll = exports.copyAndReplaceWithChangedCallback = exports.createDir = exports.resolveContents = void 0;
const fs = require("fs");
const chalk = require("chalk");
const inquirer = require("inquirer");
const path = require("path");
const mustache = require("mustache");
function walk(current) {
    if (!fs.lstatSync(current).isDirectory()) {
        return [current];
    }
    const files = fs
        .readdirSync(current)
        .map(child => walk(path.join(current, child)));
    const result = [];
    return result.concat.apply([current], files);
}
/**
 * Get a source file and replace parts of its contents.
 * @param srcPath Path to the source file.
 * @param replacements e.g. {'TextToBeReplaced': 'Replacement'}
 * @return The contents of the file with the replacements applied.
 */
function resolveContents(srcPath, replacements) {
    let content = fs.readFileSync(srcPath, 'utf8');
    if (content.includes('\r\n')) {
        // CRLF file, make sure multiline replacements are also CRLF
        for (const key of Object.keys(replacements)) {
            if (typeof replacements[key] === 'string') {
                replacements[key] = replacements[key].replace(/(?<!\r)\n/g, '\r\n');
            }
        }
    }
    else {
        // LF file, make sure multiline replacements are also LF
        for (const key of Object.keys(replacements)) {
            if (typeof replacements[key] === 'string') {
                replacements[key] = replacements[key].replace(/\r\n/g, '\n');
            }
        }
    }
    if (replacements.useMustache) {
        content = mustache.render(content, replacements);
        (replacements.regExpPatternsToRemove || []).forEach(regexPattern => {
            content = content.replace(new RegExp(regexPattern, 'g'), '');
        });
    }
    else {
        Object.keys(replacements).forEach(regex => {
            content = content.replace(new RegExp(regex, 'g'), replacements[regex]);
        });
    }
    return content;
}
exports.resolveContents = resolveContents;
// Binary files, don't process these (avoid decoding as utf8)
const binaryExtensions = ['.png', '.jar', '.keystore'];
/**
 * Copy a file to given destination, replacing parts of its contents.
 * @param srcPath Path to a file to be copied.
 * @param destPath Destination path.
 * @param replacements: e.g. {'TextToBeReplaced': 'Replacement'}
 * @param contentChangedCallback
 *        Used when upgrading projects. Based on if file contents would change
 *        when being replaced, allows the caller to specify whether the file
 *        should be replaced or not.
 *        If null, files will be overwritten.
 *        Function(path, 'identical' | 'changed' | 'new') => 'keep' | 'overwrite'
 */
async function copyAndReplace(srcPath, destPath, replacements, contentChangedCallback) {
    if (fs.lstatSync(srcPath).isDirectory()) {
        if (!fs.existsSync(destPath)) {
            fs.mkdirSync(destPath);
        }
        // Not recursive
        return;
    }
    const extension = path.extname(srcPath);
    if (binaryExtensions.indexOf(extension) !== -1) {
        // Binary file
        let shouldOverwrite = 'overwrite';
        if (contentChangedCallback) {
            const newContentBuffer = fs.readFileSync(srcPath);
            let contentChanged = 'identical';
            try {
                const origContentBuffer = fs.readFileSync(destPath);
                if (Buffer.compare(origContentBuffer, newContentBuffer) !== 0) {
                    contentChanged = 'changed';
                }
            }
            catch (err) {
                if (err.code === 'ENOENT') {
                    contentChanged = 'new';
                }
                else {
                    throw err;
                }
            }
            shouldOverwrite = await contentChangedCallback(destPath, contentChanged);
        }
        if (shouldOverwrite === 'overwrite') {
            copyBinaryFile(srcPath, destPath, err => {
                if (err) {
                    throw err;
                }
            });
        }
    }
    else {
        // Text file
        const srcPermissions = fs.statSync(srcPath).mode;
        let content = resolveContents(srcPath, replacements);
        let shouldOverwrite = 'overwrite';
        if (contentChangedCallback) {
            // Check if contents changed and ask to overwrite
            let contentChanged = 'identical';
            try {
                const origContent = fs.readFileSync(destPath, 'utf8');
                if (content !== origContent) {
                    // logger.info('Content changed: ' + destPath);
                    contentChanged = 'changed';
                }
            }
            catch (err) {
                if (err.code === 'ENOENT') {
                    contentChanged = 'new';
                }
                else {
                    throw err;
                }
            }
            shouldOverwrite = await contentChangedCallback(destPath, contentChanged);
        }
        if (shouldOverwrite === 'overwrite') {
            fs.writeFileSync(destPath, content, {
                encoding: 'utf8',
                mode: srcPermissions,
            });
        }
    }
}
/**
 * Same as 'cp' on Unix. Don't do any replacements.
 */
function copyBinaryFile(srcPath, destPath, cb) {
    let cbCalled = false;
    const srcPermissions = fs.statSync(srcPath).mode;
    const readStream = fs.createReadStream(srcPath);
    readStream.on('error', err => {
        done(err);
    });
    const writeStream = fs.createWriteStream(destPath, {
        mode: srcPermissions,
    });
    writeStream.on('error', err => {
        done(err);
    });
    writeStream.on('close', () => {
        done();
    });
    readStream.pipe(writeStream);
    function done(err) {
        if (!cbCalled) {
            cb(err);
            cbCalled = true;
        }
    }
}
function createDir(destPath) {
    if (!fs.existsSync(destPath)) {
        fs.mkdirSync(destPath);
    }
}
exports.createDir = createDir;
async function copyAndReplaceWithChangedCallback(srcPath, destRoot, relativeDestPath, replacements, alwaysOverwrite) {
    if (!replacements) {
        replacements = {};
    }
    const contentChangedCallback = alwaysOverwrite
        ? (_, contentChanged) => alwaysOverwriteContentChangedCallback(srcPath, relativeDestPath, contentChanged)
        : (_, contentChanged) => upgradeFileContentChangedCallback(srcPath, relativeDestPath, contentChanged);
    await copyAndReplace(srcPath, path.join(destRoot, relativeDestPath), replacements, contentChangedCallback);
}
exports.copyAndReplaceWithChangedCallback = copyAndReplaceWithChangedCallback;
async function copyAndReplaceAll(srcPath, destPath, relativeDestDir, replacements, alwaysOverwrite) {
    for (const absoluteSrcFilePath of walk(srcPath)) {
        const filename = path.relative(srcPath, absoluteSrcFilePath);
        const relativeDestPath = path.join(relativeDestDir, filename);
        await copyAndReplaceWithChangedCallback(absoluteSrcFilePath, destPath, relativeDestPath, replacements, alwaysOverwrite);
    }
}
exports.copyAndReplaceAll = copyAndReplaceAll;
async function alwaysOverwriteContentChangedCallback(absoluteSrcFilePath, relativeDestPath, contentChanged) {
    if (contentChanged === 'new') {
        console.log(`${chalk.bold('new')} ${relativeDestPath}`);
        return 'overwrite';
    }
    if (contentChanged === 'changed') {
        console.log(`${chalk.bold('changed')} ${relativeDestPath} ${chalk.yellow('[overwriting]')}`);
        return 'overwrite';
    }
    if (contentChanged === 'identical') {
        return 'keep';
    }
    throw new Error(`Unknown file changed state: ${relativeDestPath}, ${contentChanged}`);
}
async function upgradeFileContentChangedCallback(absoluteSrcFilePath, relativeDestPath, contentChanged) {
    if (contentChanged === 'new') {
        console.log(`${chalk.bold('new')} ${relativeDestPath}`);
        return 'overwrite';
    }
    if (contentChanged === 'changed') {
        console.log(`${chalk.bold(relativeDestPath)} ` +
            `has changed in the new version.\nDo you want to keep your ${relativeDestPath} or replace it with the ` +
            'latest version?\nMake sure you have any changes you made to this file saved somewhere.\n' +
            `You can see the new version here: ${absoluteSrcFilePath}`);
        const { shouldReplace } = await inquirer.prompt([
            {
                name: 'shouldReplace',
                type: 'confirm',
                message: `Do you want to replace ${relativeDestPath}?`,
                default: false,
            },
        ]);
        return shouldReplace ? 'overwrite' : 'keep';
    }
    if (contentChanged === 'identical') {
        return 'keep';
    }
    throw new Error(`Unknown file changed state: ${relativeDestPath}, ${contentChanged}`);
}
//# sourceMappingURL=index.js.map