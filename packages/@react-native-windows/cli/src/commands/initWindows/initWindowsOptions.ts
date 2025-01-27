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
  list?: boolean;
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
      'The native project name. Defaults to the name property in app.json or package.json',
    default: undefined,
  },
  {
    name: '--namespace [string]',
    description:
      "The native project namespace expressed using dots as separators, i.e. 'Level1.Level2.Level3'. Defaults to the same as name",
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
  {
    name: '--list',
    description:
      'Shows a list with all available templates with their descriptions.',
  },
];
