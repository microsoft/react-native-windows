/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {CommandOption} from '@react-native-community/cli-types';

export interface SetupModuleWindowsOptions {
  logging?: boolean;
  telemetry?: boolean;
  skipDeps?: boolean;
  skipBuild?: boolean;
  template?: 'cpp-lib' | 'cpp-app';
}

export const setupModuleWindowsOptions: CommandOption[] = [
  {
    name: '--logging',
    description: 'Verbose output logging',
  },
  {
    name: '--no-telemetry',
    description:
      'Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
  },
  {
    name: '--skip-deps',
    description: 'Skip dependency upgrades (use current versions)',
  },
  {
    name: '--skip-build',
    description: 'Skip final build verification step',
  },
  {
    name: '--template [string]',
    description: 'Project template (cpp-lib or cpp-app)',
    default: 'cpp-lib',
  },
];
