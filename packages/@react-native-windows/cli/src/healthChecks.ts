import path from 'path';

import { execSync} from 'child_process';

const rnDir = path.dirname(require.resolve('react-native/package.json', {paths: [process.cwd()]}));
const cliDir = path.dirname(require.resolve('@react-native-community/cli/package.json', {paths: [rnDir]}));
const execaPath = require.resolve('execa', { paths: [cliDir] });
const execa = require(execaPath);

import type {HealthCheckCategory} from '@react-native-community/cli-types';

export function getHealthChecks(): HealthCheckCategory[] | undefined {
// All our health checks are windows only...
    if (process.platform !== 'win32') {
        return undefined;
    }

    const rnwDepScriptPath = path.join(path.dirname(require.resolve(
        'react-native-windows/package.json',
        {paths: [process.cwd()]})), 'Scripts/rnw-dependencies.ps1');
    
    const rnwDeps = execSync(`powershell -ExecutionPolicy Unrestricted -NoProfile ${rnwDepScriptPath} -NoPrompt -ListChecks`);
    const deps = rnwDeps.toString().trim().split('\n');
    return [
      {
        label: 'Windows',
        healthchecks:
          deps.map(dep => {
            const match = /([^:]+): ([^:]+): (.*)/.exec(dep);
            if (!match) {
              throw new Error(`Unexpected output from ${rnwDepScriptPath}`);
            }
            const [, /*optional*/, id, name] = match;
            return {
              label: name,
              // The schema check of react-native doctor requires this to be a string, although it should be a boolean
              // Enable this once we pick up a new version of the CLI that includes https://github.com/react-native-community/cli/pull/1367
              // isRequired: (optional.trim() === 'Required') ? true : false, 
              getDiagnostics: async () => {
                let needsToBeFixed = true;
                try {
                  await execa(`powershell -ExecutionPolicy Unrestricted -NoProfile ${rnwDepScriptPath} -NoPrompt -Check ${id}`);
                  needsToBeFixed = false;
                } catch {
                }
                return {
                  needsToBeFixed,
                }
              },
              runAutomaticFix: async ({ loader, logManualInstallation }) => {
                const command = `powershell -ExecutionPolicy Unrestricted -NoProfile ${rnwDepScriptPath} -Check ${id}`;
                try {
                  const { exitCode } = await execa(command, { stdio: 'inherit' });
                  if (exitCode) {
                    logManualInstallation({ command, healthcheck: `react-native-windows dependency "${id}"` });
                    loader.fail();
                  } else {
                    loader.succeed();
                  }
                } catch {
                  logManualInstallation({ command, healthcheck: `react-native-windows dependency "${id}"` });
                  loader.fail();
                }
              }
            }
          })
      }];
  }