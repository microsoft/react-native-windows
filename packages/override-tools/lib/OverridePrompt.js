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
import * as inquirer from 'inquirer';
const WIN_PLATFORM_EXT = /(\.win32|\.windows|\.windesktop)\.js/;
export function askForDetails(overrideFile) {
    return __awaiter(this, void 0, void 0, function* () {
        switch (yield promptOverrideType()) {
            case 'derived':
                return promptDerivedDetails();
            case 'patch':
                return promptPatchDetails(overrideFile);
            case 'platform':
                return { type: 'platform' };
        }
    });
}
function promptOverrideType() {
    return __awaiter(this, void 0, void 0, function* () {
        const response = yield inquirer.prompt([
            {
                type: 'list',
                name: 'type',
                message: 'Override type:',
                choices: [
                    {
                        name: 'Derived from upstream      (E.g. a Windows implementation of an existing component)',
                        value: 'derived',
                        short: 'Derived',
                    },
                    {
                        name: 'Patches to upstream        (E.g. commenting out code)',
                        value: 'patch',
                        short: 'Patch',
                    },
                    {
                        name: 'Independent platform logic (E.g. Windows-specific modules)',
                        value: 'platform',
                        short: 'Platform',
                    },
                ],
            },
        ]);
        return response.type;
    });
}
function promptDerivedDetails() {
    return __awaiter(this, void 0, void 0, function* () {
        let issue;
        const copiedResponse = yield inquirer.prompt([
            {
                type: 'confirm',
                name: 'codeCopied',
                default: true,
                message: 'Does the derived file copy code from upstream?',
            },
        ]);
        if (copiedResponse.codeCopied) {
            console.log('Copying code from upstream adds significant technical debt. Please create an issue using the "Deforking" label to track making changes so that code can be shared.');
            const issueResponse = yield inquirer.prompt([
                {
                    type: 'input',
                    validate: validateIssueNumber,
                    name: 'issue',
                    message: 'Github Issue Number:',
                },
            ]);
            issue = issueResponse.issue;
        }
        const baseFileResponse = yield inquirer.prompt([
            {
                type: 'input',
                name: 'baseFile',
                message: 'What file does this override derive from (pick the closest)?',
            },
        ]);
        return { type: 'derived', baseFile: baseFileResponse.baseFile, issue };
    });
}
function promptPatchDetails(overrideFile) {
    return __awaiter(this, void 0, void 0, function* () {
        console.log('Patch-style overrides add signficant technical debt. Please create an issue using the "Deforking" label to track removal of the patch.');
        const response = yield inquirer.prompt([
            {
                type: 'input',
                validate: validateIssueNumber,
                name: 'issue',
                message: 'Github Issue Number:',
            },
        ]);
        const baseFile = overrideFile.replace(WIN_PLATFORM_EXT, '.js');
        return { type: 'patch', baseFile, issue: response.issue };
    });
}
function validateIssueNumber(answer) {
    return (Number.isInteger(Number.parseInt(answer)) || 'Github issue must be a number');
}
//# sourceMappingURL=OverridePrompt.js.map