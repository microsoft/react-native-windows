/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const child_process = require('child_process');
const ora = require('ora');
const spinners = require('cli-spinners');
const chalk = require('chalk');

function setSpinnerText(spinner, prefix, text) {
  text = prefix + spinnerString(text);
  spinner.text = text.length > 45 ? text.slice(0, 110) + '...' : text;
}

function spinnerString(msg) {
  msg = msg.trim();
  const lastLineIndex = msg.lastIndexOf('\n');
  if (lastLineIndex > 0) {
    msg = msg.slice(lastLineIndex + 1);
  }
  msg = msg.trim();
  return msg;
}

function newSpinner(text) {
  const options = {text: text, indent: 1};

  // ora turns off emoji rendering by default on windows, since the default terminal
  // doesn't render them.  Turn them back on when running in the new windows terminal
  if (process.env.WT_SESSION) {
    options.spinner = spinners.dots;
  }

  // By default, ora process.stderr as the output stream, however,the VS Code debugger
  // Uses stdout to match success patterns
  options.stream = process.stdout;

  return ora(options).start();
}

function commandWithProgress(spinner, taskDoingName, command, args, verbose) {
  return new Promise(function(resolve, reject) {
    const spawnOptions = verbose ? {stdio: 'inherit'} : {};

    if (verbose) {
      spinner.stop();
    }

    const cp = child_process.spawn(command, args, spawnOptions);

    if (!verbose) {
      cp.stdout.on('data', chunk => {
        const text = chunk.toString();
        setSpinnerText(spinner, taskDoingName + ': ', text);
      });
      cp.stderr.on('data', chunk => {
        const text = chunk.toString();
        if (verbose) {
          console.error(chalk.red(text));
        }
        setSpinnerText(spinner, taskDoingName + ': ERROR: ', text);
      });
    }
    cp.on('error', e => {
      if (verbose) {
        console.error(chalk.red(e.toString()));
      }
      spinner.fail(e.toString());
      reject(e);
    }).on('close', function(code) {
      if (code === 0) {
        spinner.succeed(taskDoingName);
        resolve();
      } else {
        spinner.fail();
        reject();
      }
    });
  });
}

function newError(text) {
  newSpinner(text).fail(text);
}

function newWarn(text) {
  newSpinner(text).warn(text);
}

function newSuccess(text) {
  newSpinner(text).succeed(text);
}

function newInfo(text) {
  newSpinner(text).info(text);
}

module.exports = {
  commandWithProgress,
  newError,
  newInfo,
  newSpinner,
  newSuccess,
  newWarn,
};
