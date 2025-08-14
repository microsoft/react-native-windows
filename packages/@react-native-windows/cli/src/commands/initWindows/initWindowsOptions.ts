/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {CommandOption} from '@react-native-community/cli-types';

export interface InitOptions {
  logging?: boolean;
  template?: string;
  name?: string;
  namespace?: string;
  overwrite?: boolean;
  telemetry?: boolean;
  list?: boolean;
  prompt?: boolean;
}

export const initOptions: CommandOption[] = [
  {
    name: '--logging',
    description: 'Verbose output logging',
  },
  {
    name: '--template [string]',
    description: 'Specify the template to use',
    default: 'old/uwp-cpp-app',
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
  {
    name: '--prompt',
    description: 'Show interactive prompts for Old Arch to New Arch switch',
    default: true,
  },
  {
    name: '--no-prompt',
    description:
      'Skip interactive prompt for Old Arch to New Arch switch warning and use default choices',
  },
];
