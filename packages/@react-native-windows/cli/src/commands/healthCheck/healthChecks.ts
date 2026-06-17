/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';

import * as pathHelpers from '../../utils/pathHelpers';

import execa from 'execa';

import type {
  HealthCheckCategory,
  HealthCheckInterface,
} from '@react-native-community/cli-doctor/build/types';
import {powershell} from '../../utils/commandWithProgress';
import {HealthCheckList} from './healthCheckList';

export function getHealthChecks(): HealthCheckCategory[] | undefined {
  // #8471: There are known cases where the dependencies script will error out.
  // Fail gracefully if that happens in the meantime.
  try {
    return getHealthChecksUnsafe();
  } catch {
    return [
      {
        label: 'Windows',
        healthchecks: [
          {
            label: 'Failed to enumerate health checks',
            description: 'Failed to enumerate health checks',
            getDiagnostics: async () => ({needsToBeFixed: true}),
            runAutomaticFix: async ({loader}) => {
              loader.fail();
            },
          },
        ],
      },
    ];
  }
}

function getHealthChecksUnsafe(): HealthCheckCategory[] | undefined {
  // All our health checks are windows only...
  if (process.platform !== 'win32') {
    return undefined;
  }

  const rnwDepScriptPath = path.join(
    pathHelpers.resolveRnwRoot(process.cwd()),
    'Scripts/rnw-dependencies.ps1',
  );

  return [
    {
      label: 'Windows',
      healthchecks: HealthCheckList.map((dep: [boolean, string, string]) => {
        const [optional, id, name] = dep;
        return {
          label: name,
          description: name,
          // The schema check of @react-native-community/cli doctor requires this to be a string, although it should be a boolean
          // Enable this once we pick up a new version of the CLI that includes https://github.com/react-native-community/cli/pull/1367
          isRequired: optional,
          getDiagnostics: async () => {
            let needsToBeFixed = true;
            try {
              await execa(
                `"${powershell}" -ExecutionPolicy Unrestricted -NoProfile "${rnwDepScriptPath}" -NoPrompt -Check ${id}`,
              );
              needsToBeFixed = false;
            } catch {}
            return {
              needsToBeFixed,
            };
          },
          runAutomaticFix: async ({loader, logManualInstallation}) => {
            const command = `${powershell} -ExecutionPolicy Unrestricted -NoProfile "${rnwDepScriptPath}" -Check ${id}`;
            try {
              const {exitCode} = await execa(command, {stdio: 'inherit'});
              if (exitCode) {
                logManualInstallation({
                  command,
                  healthcheck: `react-native-windows dependency "${id}"`,
                });
                loader.fail();
              } else {
                loader.succeed();
              }
            } catch {
              logManualInstallation({
                command,
                healthcheck: `react-native-windows dependency "${id}"`,
              });
              loader.fail();
            }
          },
        } as HealthCheckInterface;
      }),
    },
  ];
}
