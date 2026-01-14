/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

const fs = require('fs');
const path = require('path');
const {execSync, spawnSync} = require('child_process');
const {task} = require('just-scripts');

function registerNuGetRestoreTask(options) {
  const config = normalizeOptions(options);
  task(config.taskName, () => executeNuGetRestore(config));
}

function runNuGetRestore(options) {
  const config = normalizeOptions(options);
  executeNuGetRestore(config);
}

function executeNuGetRestore(config) {
  if (process.platform !== 'win32') {
    console.log('Skipping NuGet restore on non-Windows host');
    return;
  }

  if (!fs.existsSync(config.scriptPath)) {
    console.warn(`NuGet restore script not found: ${config.scriptPath}`);
    return;
  }

  const vsDevCmd = findVsDevCmd();

  fs.mkdirSync(config.logDirectory, {recursive: true});
  pruneRestoreLogs(config.logDirectory, config.maxLogCount);

  const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
  const logPath = path.join(
    config.logDirectory,
    `NuGetRestore-${timestamp}.log`,
  );

  console.log(
    `Restoring NuGet packages (log: ${path.relative(process.cwd(), logPath)})`,
  );

  const scriptArgs = config.scriptArguments.length
    ? ` ${config.scriptArguments.join(' ')}`
    : '';
  const restoreCommand = `call ${quote(
    vsDevCmd,
  )} && powershell -NoProfile -ExecutionPolicy Bypass -File ${quote(
    config.scriptPath,
  )}${scriptArgs}`;
  const wrappedCommand = `${restoreCommand}`;

  writeLogHeader(
    logPath,
    restoreCommand,
    config.workingDirectory,
    config.scriptArguments,
  );

  const result = spawnSync('cmd.exe', ['/d', '/s', '/c', wrappedCommand], {
    encoding: 'utf8',
    windowsHide: true,
    cwd: config.workingDirectory,
    windowsVerbatimArguments: true,
  });

  appendProcessOutput(logPath, result);

  if (result.error) {
    throw new Error(`Failed to start NuGet restore: ${result.error.message}`);
  }

  if (result.status !== 0) {
    const tail = readLogTail(logPath);
    throw new Error(
      `NuGet restore failed. See ${path.relative(
        process.cwd(),
        logPath,
      )} for details.\n${tail}`,
    );
  }

  console.log('NuGet restore completed');
}

function findVsDevCmd() {
  const programFilesX86 =
    process.env['ProgramFiles(x86)'] || process.env.ProgramFiles;

  if (!programFilesX86) {
    throw new Error('Program Files directory not found in environment');
  }

  const vsWherePath = path.join(
    programFilesX86,
    'Microsoft Visual Studio',
    'Installer',
    'vswhere.exe',
  );

  if (!fs.existsSync(vsWherePath)) {
    throw new Error(
      'vswhere.exe not found. Install Visual Studio 2022 (or Build Tools).',
    );
  }

  let installationRoot = '';
  try {
    installationRoot = execSync(
      `${quote(vsWherePath)} -latest -prerelease -products * -requires Microsoft.Component.MSBuild -property installationPath -format value`,
      {encoding: 'utf8'},
    )
      .split(/\r?\n/)
      .find(Boolean);
  } catch (error) {
    throw new Error(`vswhere.exe failed: ${error.message}`);
  }

  if (!installationRoot) {
    throw new Error('No Visual Studio installation with MSBuild found');
  }

  const vsDevCmd = path.join(
    installationRoot.trim(),
    'Common7',
    'Tools',
    'VsDevCmd.bat',
  );

  if (!fs.existsSync(vsDevCmd)) {
    throw new Error(`VsDevCmd.bat not found at ${vsDevCmd}`);
  }

  return vsDevCmd;
}

function pruneRestoreLogs(logDir, maxLogCount) {
  const files = fs
    .readdirSync(logDir)
    .filter(file => file.startsWith('NuGetRestore-') && file.endsWith('.log'))
    .sort();

  while (files.length > maxLogCount) {
    const oldest = files.shift();
    if (oldest) {
      fs.rmSync(path.join(logDir, oldest), {force: true});
    }
  }
}

function writeLogHeader(logPath, command, cwd, scriptArguments) {
  const header = [
    `Command line : ${command}`,
    `Working dir  : ${cwd}`,
    `Script args  : ${
      scriptArguments && scriptArguments.length
        ? scriptArguments.join(' ')
        : '(none)'
    }`,
    `Timestamp    : ${new Date().toISOString()}`,
    '',
  ].join('\n');

  fs.writeFileSync(logPath, `${header}\n`, {encoding: 'utf8'});
}

function appendProcessOutput(logPath, result) {
  let output = '';

  if (result.stdout) {
    output += result.stdout;
  }
  if (result.stderr) {
    output += result.stderr;
  }

  if (!output) {
    return;
  }

  fs.appendFileSync(logPath, output, {encoding: 'utf8'});
}

function readLogTail(logPath, lineCount = 40) {
  try {
    const contents = fs.readFileSync(logPath, 'utf8');
    const lines = contents.trim().split(/\r?\n/);
    return lines.slice(-lineCount).join('\n');
  } catch (error) {
    return `Could not read log tail: ${error.message}`;
  }
}

function normalizeOptions(options = {}) {
  if (!options.scriptPath) {
    throw new Error('scriptPath is required for NuGet restore task');
  }

  const resolvedScriptPath = path.resolve(options.scriptPath);
  const resolvedLogDir = options.logDirectory
    ? path.resolve(options.logDirectory)
    : path.join(path.dirname(resolvedScriptPath), 'logs');

  return {
    taskName: options.taskName || 'restoreNuGetPackages',
    scriptPath: resolvedScriptPath,
    logDirectory: resolvedLogDir,
    workingDirectory: options.workingDirectory
      ? path.resolve(options.workingDirectory)
      : path.dirname(resolvedScriptPath),
    maxLogCount: options.maxLogCount ?? 5,
    scriptArguments: Array.isArray(options.scriptArguments)
      ? options.scriptArguments
      : [],
  };
}

function quote(value) {
  return `"${value}"`;
}

module.exports = {
  registerNuGetRestoreTask,
  runNuGetRestore,
};
