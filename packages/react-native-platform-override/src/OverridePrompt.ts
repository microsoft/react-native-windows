/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as inquirer from 'inquirer';

import {OverrideType} from './Manifest';

const WIN_PLATFORM_EXT = /(\.win32|\.windows|\.windesktop)\.js/;

export interface Details {
  type: OverrideType;
  baseFile?: string;
  issue?: number;
}

export async function askForDetails(overrideFile: string): Promise<Details> {
  switch (await promptOverrideType()) {
    case 'derived':
      return promptDerivedDetails();
    case 'patch':
      return promptPatchDetails(overrideFile);
    case 'platform':
      return {type: 'platform'};
  }
}

async function promptOverrideType(): Promise<OverrideType> {
  const response = await inquirer.prompt([
    {
      type: 'list',
      name: 'type',
      message: 'Override type:',
      choices: [
        {
          name:
            'Derived from upstream      (E.g. a Windows implementation of an existing component)',
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
}

async function promptDerivedDetails(): Promise<Details> {
  let issue: number;

  const copiedResponse = await inquirer.prompt([
    {
      type: 'confirm',
      name: 'codeCopied',
      default: true,
      message: 'Does the derived file copy code from upstream?',
    },
  ]);

  if (copiedResponse.codeCopied) {
    console.log(
      'Copying code from upstream adds significant technical debt. Please create an issue using the "Deforking" label to track making changes so that code can be shared.',
    );
    const issueResponse = await inquirer.prompt([
      {
        type: 'input',
        validate: validateIssueNumber,
        name: 'issue',
        message: 'Github Issue Number:',
      },
    ]);
    issue = Number.parseInt(issueResponse.issue, 10);
  }

  const baseFileResponse = await inquirer.prompt([
    {
      type: 'input',
      name: 'baseFile',
      message: 'What file does this override derive from (pick the closest)?',
    },
  ]);

  return {type: 'derived', baseFile: baseFileResponse.baseFile, issue};
}

async function promptPatchDetails(overrideFile: string): Promise<Details> {
  console.log(
    'Patch-style overrides add signficant technical debt. Please create an issue using the "Deforking" label to track removal of the patch.',
  );
  const response = await inquirer.prompt([
    {
      type: 'input',
      validate: validateIssueNumber,
      name: 'issue',
      message: 'Github Issue Number:',
    },
  ]);

  const baseFile = overrideFile.replace(WIN_PLATFORM_EXT, '.js');
  return {type: 'patch', baseFile, issue: Number.parseInt(response.issue, 10)};
}

function validateIssueNumber(answer: string): boolean | string {
  return (
    Number.isInteger(Number.parseInt(answer, 10)) ||
    'Github issue must be a number'
  );
}
