const build = require('./build');
const chalk = require('chalk');
const execSync = require('child_process').execSync;
const glob = require('glob');
const path = require('path');

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'node_modules/react-native-windows/ReactWindows/ReactNative.CodeGen.sln'))[0];
}

function getExeFile(options, configuration) {
  return glob.sync(path.join(options.root, `node_modules/react-native-windows/ReactWindows/ReactNative.CodeGen/bin/${configuration}/ReactNative.CodeGen.exe`))[0];
}

function generate(options) {
  console.log(chalk.green('Building ReactNative.CodeGen.csproj'));
  const slnFile = getSolutionFile(options);
  if (!slnFile) {
    console.error(chalk.red('ReactNative.CodeGen.sln not found.'));
    return;
  }

  try {
    build.restoreNuGetPackages(options, slnFile, options.verbose);
  } catch (e) {
    console.error(chalk.red('Failed to restore the NuGet packages'));
    return;
  }

  const buildType = options.release ? 'Release' : 'Debug';

  try {
    build.buildSolution(slnFile, buildType, 'Any CPU', options.verbose);
  } catch (e) {
    console.error(chalk.red(`Build failed with message ${e}. Check your build configuration.`));
    return;
  }

  console.log(chalk.green('Running ReactNative.CodeGen.exe'));
  const exeFile = getExeFile(options, buildType);
  if (!exeFile) {
    console.error(chalk.red('Could not find build output for codegen project.'));
    return;
  }

  execSync(exeFile, { stdio: 'inherit' });
}

module.exports = {
  generate,
};
