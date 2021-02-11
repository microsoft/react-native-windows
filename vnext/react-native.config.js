// @ts-check
require('source-map-support').install();

const cli = require('@react-native-windows/cli');
const { execSync } = require('child_process');
const path = require('path');

const cliDir = path.dirname(require.resolve('@react-native-community/cli/package.json'));
const execaPath = require.resolve('execa', { paths: [cliDir] });
const execa = require(execaPath);

module.exports = {
  // **** This section defined commands and options on how to provide the Windows platform to external applications
  commands: cli.commands,
  platforms: {
    windows: {
      linkConfig: () => null,
      projectConfig: cli.projectConfig,
      dependencyConfig: cli.dependencyConfig,
      npmPackageName: 'react-native-windows',
    },
  }
};

// All our health checks are windows only...
if (process.platform === 'win32') {
  const rnwDepScriptPath = path.resolve(__dirname, 'Scripts/rnw-dependencies.ps1');
  try {
    const rnwDeps = execSync(`powershell ${rnwDepScriptPath} -NoPrompt -ListChecks`);
    const deps = rnwDeps.toString().trim().split('\n');
    module.exports.healthChecks = [
      {
        label: 'react-native-windows',
        healthchecks:
          deps.map(dep => {
            const match = /([^:]+): ([^:]+): (.*)/.exec(dep);
            if (!match) {
              throw new Error(`Unexpected output from ${rnwDepScriptPath}`);
            }
            const [other, optional, id, name] = match;
            return {
              label: name,
              // The schema check of react-native doctor requires this to be a string, although it should be a boolean 
              // It also doesn't seem to do much with the value currently
              // isRequired: (optional.trim() === 'Required') ? true : false, 
              getDiagnostics: async () => {
                let needsToBeFixed = true;
                try {
                  await execa(`powershell ${rnwDepScriptPath} -NoPrompt -Check ${id}`);
                  needsToBeFixed = false;
                } catch {
                }
                return {
                  needsToBeFixed,
                }
              },
              runAutomaticFix: async ({ loader, logManualInstallation }) => {
                const command = `powershell ${rnwDepScriptPath} -Check ${id}`;
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
      }]
  }
  catch {
  }
}