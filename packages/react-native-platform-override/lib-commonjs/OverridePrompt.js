"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.promptForOverrideDetails = exports.overrideFromDetails = void 0;
const FileSearch = require("./FileSearch");
const inquirer = require("inquirer");
const path = require("path");
async function overrideFromDetails(overridePath, answers, factory) {
    const manifestPath = await FileSearch.findManifest(overridePath);
    const manifestDir = path.dirname(manifestPath);
    const overrideName = path.normalize(path.relative(manifestDir, overridePath));
    switch (answers.type) {
        case 'derived':
            return factory.createDerivedOverride(overrideName, answers.baseFile, answers.issue);
        case 'patch':
            return factory.createPatchOverride(overrideName, answers.baseFile, answers.issue);
        case 'platform':
            return factory.createPlatformOverride(overridePath);
        case 'copy':
            return factory.createCopyOverride(overrideName, answers.baseFile, answers.issue);
        case 'directoryCopy':
            return factory.createDirectoryCopyOverride(overrideName, answers.baseDirectory, answers.issue);
    }
}
exports.overrideFromDetails = overrideFromDetails;
async function promptForOverrideDetails() {
    return inquirer.prompt([
        {
            type: 'list',
            name: 'type',
            message: 'Override type:',
            choices: [
                {
                    name: 'Derived from upstream            (E.g. a Windows implementation of an existing component)',
                    value: 'derived',
                    short: 'Derived',
                },
                {
                    name: 'Patches to upstream              (E.g. commenting out code)',
                    value: 'patch',
                    short: 'Patch',
                },
                {
                    name: 'Independent platform logic       (E.g. Windows-specific modules)',
                    value: 'platform',
                    short: 'Platform',
                },
                {
                    name: 'Copy of an upstream file',
                    value: 'copy',
                    short: 'Copy',
                },
                {
                    name: 'Copy of an upstream directory',
                    value: 'directoryCopy',
                    short: 'Directory Copy',
                },
            ],
        },
        {
            when: res => res.type === 'derived',
            type: 'confirm',
            name: 'codeCopied',
            default: true,
            message: 'Does the derived file copy code from upstream?',
        },
        {
            when: res => (res.type === 'derived' && res.codeCopied) ||
                ['copy', 'directoryCopy', 'patch'].includes(res.type),
            type: 'input',
            validate: validateIssueNumber,
            filter: filterIssueNumber,
            askAnswered: true,
            name: 'issue',
            message: 'Issue number tracking removal:',
        },
        {
            when: res => ['copy', 'patch', 'derived'].includes(res.type),
            type: 'input',
            name: 'baseFile',
            message: 'What file is this override based off of?',
        },
        {
            when: res => res.type === 'directoryCopy',
            type: 'input',
            name: 'baseDirectory',
            message: 'What directory are you copying from?',
        },
    ]);
}
exports.promptForOverrideDetails = promptForOverrideDetails;
function filterIssueNumber(answer) {
    const parsedInt = Number.parseInt(answer, 10);
    return Number.isInteger(parsedInt) ? parsedInt : '';
}
function validateIssueNumber(answer) {
    return (Number.isInteger(Number.parseInt(answer, 10)) || 'Issue must be a number');
}
//# sourceMappingURL=OverridePrompt.js.map