/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {CommandOption} from '@react-native-community/cli-types';

export interface ModuleWindowsSetupOptions {
  logging?: boolean;
  name?: string;
  overwrite?: boolean;
  telemetry?: boolean;
}

export const moduleWindowsSetupOptions: CommandOption[] = [
  {
    name: '--logging',
    description: 'Verbose output logging',
  },
  {
    name: '--name [string]',
    description: 'Override the module name (defaults to package.json name)',
  },
  {
    name: '--overwrite',
    description: 'Overwrite existing files without confirmation',
  },
  {
    name: '--no-telemetry',
    description:
      'Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
  },
];
