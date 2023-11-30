/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {CommandOption} from '@react-native-community/cli-types';

export interface AutoLinkOptions {
  logging?: boolean;
  check?: boolean;
  sln?: string;
  proj?: string;
  telemetry?: boolean;
}

export const autolinkOptions: CommandOption[] = [
  {
    name: '--logging',
    description: 'Verbose output logging',
  },
  {
    name: '--check',
    description: 'Only check whether any autolinked files need to change',
  },
  {
    name: '--sln [string]',
    description:
      "Override the app solution file determined by 'react-native config', e.g. windows\\myApp.sln",
    default: undefined,
  },
  {
    name: '--proj [string]',
    description:
      "Override the app project file determined by 'react-native config', e.g. windows\\myApp\\myApp.vcxproj",
    default: undefined,
  },
  {
    name: '--no-telemetry',
    description:
      'Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
  },
];
