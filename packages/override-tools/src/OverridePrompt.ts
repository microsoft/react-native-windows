/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as prompts from 'prompts';

import {OverrideType} from './Manifest';

const PROMPT_OPTIONS: prompts.Options = {
  onCancel: () => process.exit(1),
};

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
  const response = await prompts(
    {
      type: 'select',
      name: 'type',
      message: 'Override type:',
      choices: [
        {
          title: 'Derived from upstream',
          // @ts-ignore description is missing from typings
          description: 'E.g. a Windows implementation of an existing component',
          value: 'derived',
        },
        {
          title: 'Patches to upstream',
          // @ts-ignore description is missing from typings
          description: 'E.g. commenting out code',
          value: 'patch',
        },
        {
          title: 'Independent platform logic',
          // @ts-ignore description is missing from typings
          description: 'E.g. Windows-specific modules',
          value: 'platform',
        },
      ],
    },
    PROMPT_OPTIONS,
  );

  return response.type;
}

async function promptDerivedDetails(): Promise<Details> {
  let issue: number;

  const copiedResponse = await prompts(
    {
      type: 'toggle',
      name: 'codeCopied',
      active: 'Yes',
      inactive: 'No',
      initial: true,
      message: 'Does the derived file copy code from upstream?',
    },
    PROMPT_OPTIONS,
  );

  if (copiedResponse.codeCopied) {
    console.log(
      'Copying code from upstream adds significant technical debt. Please create an issue using the "Deforking" label to track making changes so that code can be shared.',
    );
    const issueResponse = await prompts(
      {
        type: 'number',
        name: 'issue',
        message: 'Github Issue:',
      },
      PROMPT_OPTIONS,
    );
    issue = issueResponse.issue;
  }

  const baseFileResponse = await prompts(
    {
      type: 'text',
      name: 'baseFile',
      message: 'What file does this override derive from (pick the closest)?',
    },
    PROMPT_OPTIONS,
  );

  return {type: 'derived', baseFile: baseFileResponse.baseFile, issue};
}

async function promptPatchDetails(overrideFile: string): Promise<Details> {
  console.log(
    'Patch-style overrides add signficant technical debt. Please create an issue using the "Deforking" label to track removal of the patch.',
  );
  const response = await prompts(
    {
      type: 'number',
      name: 'issue',
      message: 'Github Issue:',
    },
    PROMPT_OPTIONS,
  );

  const baseFile = overrideFile.replace(WIN_PLATFORM_EXT, '.js');
  return {type: 'patch', baseFile, issue: response.issue};
}
