/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as inquirer from 'inquirer';

import Override from './Override';
import OverrideFactory from './OverrideFactory';

export type OverridePromptAnswers =
  | {type: 'derived'; baseFile: string; codeCopied: boolean; issue?: number}
  | {type: 'patch'; baseFile: string; issue: number}
  | {type: 'platform'}
  | {type: 'copy'; baseFile: string; issue: number};

export async function overrideFromDetails(
  overridePath: string,
  answers: OverridePromptAnswers,
  factory: OverrideFactory,
): Promise<Override> {
  switch (answers.type) {
    case 'derived':
      return factory.createDerivedOverride(
        overridePath,
        answers.baseFile,
        answers.issue,
      );
    case 'patch':
      return factory.createPatchOverride(
        overridePath,
        answers.baseFile,
        answers.issue,
      );
    case 'platform':
      return factory.createPlatformOverride(overridePath);
    case 'copy':
      return factory.createCopyOverride(
        overridePath,
        answers.baseFile,
        answers.issue,
      );
  }
}

export async function promptForOverrideDetails(): Promise<
  OverridePromptAnswers
> {
  return inquirer.prompt([
    {
      type: 'list',
      name: 'type',
      message: 'Override type:',
      choices: [
        {
          name:
            'Derived from upstream            (E.g. a Windows implementation of an existing component)',
          value: 'derived',
          short: 'Derived',
        },
        {
          name: 'Patches to upstream              (E.g. commenting out code)',
          value: 'patch',
          short: 'Patch',
        },
        {
          name:
            'Independent platform logic       (E.g. Windows-specific modules)',
          value: 'platform',
          short: 'Platform',
        },
        {
          name: 'Exact copy of an upstream file',
          value: 'copy',
          short: 'Copy',
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
      when: res =>
        (res.type === 'derived' && res.codeCopied) ||
        ['copy', 'patch'].includes(res.type),
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
  ]);
}

function filterIssueNumber(answer: string): number | string {
  const parsedInt = Number.parseInt(answer, 10);
  return Number.isInteger(parsedInt) ? parsedInt : '';
}

function validateIssueNumber(answer: string): boolean | string {
  return (
    Number.isInteger(Number.parseInt(answer, 10)) || 'Issue must be a number'
  );
}
