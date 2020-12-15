/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {spawn, SpawnOptions} from 'child_process';
import * as ora from 'ora';
import * as spinners from 'cli-spinners';
import * as chalk from 'chalk';
import {CodedError, CodedErrorType} from '@react-native-windows/telemetry';

function setSpinnerText(spinner: ora.Ora, prefix: string, text: string) {
  text = prefix + spinnerString(text);
  spinner.text = text.length > 45 ? text.slice(0, 110) + '...' : text;
}

function spinnerString(msg: string) {
  msg = msg.trim();
  const lastLineIndex = msg.lastIndexOf('\n');
  if (lastLineIndex > 0) {
    msg = msg.slice(lastLineIndex + 1);
  }
  msg = msg.trim();
  return msg;
}

export function newSpinner(text: string) {
  const options: ora.Options = {
    text: text,
    indent: 1,

    // ora turns off emoji rendering by default on windows, since the default terminal
    // doesn't render them.  Turn them back on when running in the new windows terminal
    ...(process.env.WT_SESSION && {spinner: spinners.dots}),

    // By default, ora process.stderr as the output stream, however,the VS Code debugger
    // Uses stdout to match success patterns
    stream: process.stdout,
  };

  return ora(options).start();
}

export async function runPowerShellScriptFunction(
  taskDescription: string,
  script: string | null,
  funcName: string,
  verbose: boolean,
  errorCategory: CodedErrorType,
) {
  try {
    const printException = verbose ? '$_;' : '';
    const importScript = script ? `Import-Module "${script}"; ` : '';
    await commandWithProgress(
      newSpinner(taskDescription),
      taskDescription,
      'powershell',
      [
        '-NoProfile',
        '-ExecutionPolicy',
        'RemoteSigned',
        `${importScript}try { ${funcName} -ErrorAction Stop; $lec = $LASTEXITCODE; } catch { $lec = 1; ${printException} }; exit $lec`,
      ],
      verbose,
      errorCategory,
    );
  } catch {
    // The error output from the process will be shown if verbose is set.
    // We don't capture the process output if verbose is set, but at least we have the task name in text, so throw that.
    throw new CodedError(errorCategory, taskDescription);
  }
}

export function commandWithProgress(
  spinner: ora.Ora,
  taskDoingName: string,
  command: string,
  args: string[],
  verbose: boolean,
  errorCategory: CodedErrorType,
) {
  return new Promise((resolve, reject) => {
    const spawnOptions: SpawnOptions = verbose ? {stdio: 'inherit'} : {};

    if (verbose) {
      spinner.stop();
    }

    const cp = spawn(command, args, spawnOptions);
    let firstErrorLine: string | null = null;
    if (!verbose) {
      cp.stdout!.on('data', chunk => {
        const text = chunk.toString();
        setSpinnerText(spinner, taskDoingName + ': ', text);
      });
      cp.stderr!.on('data', chunk => {
        const text: string = chunk.toString();
        if (!firstErrorLine) {
          firstErrorLine = text;
        }
        if (text) {
          setSpinnerText(spinner, taskDoingName + ': ERROR: ', firstErrorLine);
        }
        reject(
          new CodedError(errorCategory, firstErrorLine, {
            taskName: taskDoingName,
          }),
        );
      });
    }
    cp.on('error', e => {
      if (verbose) {
        console.error(chalk.red(e.toString()));
      }
      spinner.fail(e.toString());
      const ce = new CodedError(errorCategory, e.message);
      ce.stack = e.stack;
      reject(ce);
    }).on('close', code => {
      if (code === 0) {
        spinner.succeed(taskDoingName);
        resolve();
      } else {
        spinner.fail();
        reject(
          new CodedError(
            errorCategory,
            `${taskDoingName} - error code ${code}`,
            {
              errorCode: code,
            },
          ),
        );
      }
    });
  });
}

export function newError(text: string) {
  newSpinner(text).fail(text);
}

export function newWarn(text: string) {
  newSpinner(text).warn(text);
}

export function newSuccess(text: string) {
  newSpinner(text).succeed(text);
}

export function newInfo(text: string) {
  newSpinner(text).info(text);
}
