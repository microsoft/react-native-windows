/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {CommandOption} from '@react-native-community/cli-types';

export interface InitOptions {
  logging?: boolean;
  template?: string;
  name?: string;
  namespace?: string;
  overwrite?: boolean;
  telemetry?: boolean;
}

export const initOptions: CommandOption[] = [
  {
    name: '--logging',
    description: 'Verbose output logging',
  },
  {
    name: '--template [string]',
    description: 'Specify the template to use',
    default: undefined,
  },
  {
    name: '--name [string]',
    description:
      'The native project name. Defaults to the name property iin app.json or package.json',
    default: undefined,
  },
  {
    name: '--namespace [string]',
    description:
      "The native project namespace. This should be expressed using dots as separators. i.e. 'Level1.Level2.Level3'. The generator will apply the correct syntax for the target language.",
    default: undefined,
  },
  {
    name: '--overwrite',
    description: 'Overwrite any existing files without prompting',
  },
  {
    name: '--no-telemetry',
    description:
      'Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
  },
];
