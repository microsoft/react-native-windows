/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {CommandOption} from '@react-native-community/cli-types';

export interface ModuleWindowsSetupOptions {
  logging?: boolean;
  telemetry?: boolean;
  skipDeps?: boolean;
  skipBuild?: boolean;
}

export const moduleWindowsSetupOptions: CommandOption[] = [
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
];