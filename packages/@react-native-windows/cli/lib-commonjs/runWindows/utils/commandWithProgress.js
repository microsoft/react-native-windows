"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.newInfo = exports.newSuccess = exports.newWarn = exports.newError = exports.commandWithProgress = exports.runPowerShellScriptFunction = exports.newSpinner = void 0;
const child_process_1 = require("child_process");
const ora = require("ora");
const spinners = require("cli-spinners");
const chalk = require("chalk");
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
    const options = Object.assign(Object.assign({ text: text, indent: 1 }, (process.env.WT_SESSION && { spinner: spinners.dots })), { 
        // By default, ora process.stderr as the output stream, however,the VS Code debugger
        // Uses stdout to match success patterns
        stream: process.stdout });
    return ora(options).start();
}
exports.newSpinner = newSpinner;
async function runPowerShellScriptFunction(taskDescription, script, funcName, verbose) {
    try {
        const printException = verbose ? '$_;' : '';
        const importScript = script ? `Import-Module "${script}"; ` : '';
        await commandWithProgress(newSpinner(taskDescription), taskDescription, 'powershell', [
            '-NoProfile',
            '-ExecutionPolicy',
            'RemoteSigned',
            `${importScript}try { ${funcName} -ErrorAction Stop; $lec = $LASTEXITCODE; } catch { $lec = 1; ${printException} }; exit $lec`,
        ], verbose);
    }
    catch (_a) {
        // The error output from the process will be shown if verbose is set.
        // We don't capture the process output if verbose is set, but at least we have the task name in text, so throw that.
        throw new Error(taskDescription);
    }
}
exports.runPowerShellScriptFunction = runPowerShellScriptFunction;
function commandWithProgress(spinner, taskDoingName, command, args, verbose) {
    return new Promise(function (resolve, reject) {
        const spawnOptions = verbose ? { stdio: 'inherit' } : {};
        if (verbose) {
            spinner.stop();
        }
        const cp = child_process_1.spawn(command, args, spawnOptions);
        let firstErrorLine = null;
        if (!verbose) {
            cp.stdout.on('data', chunk => {
                const text = chunk.toString();
                setSpinnerText(spinner, taskDoingName + ': ', text);
            });
            cp.stderr.on('data', chunk => {
                const text = chunk.toString();
                if (!firstErrorLine) {
                    firstErrorLine = text;
                }
                if (verbose) {
                    console.error(chalk.red(text));
                }
                if (text) {
                    setSpinnerText(spinner, taskDoingName + ': ERROR: ', firstErrorLine);
                }
                reject(new Error(firstErrorLine));
            });
        }
        cp.on('error', e => {
            if (verbose) {
                console.error(chalk.red(e.toString()));
            }
            spinner.fail(e.toString());
            reject(e);
        }).on('close', function (code) {
            if (code === 0) {
                spinner.succeed(taskDoingName);
                resolve();
            }
            else {
                spinner.fail();
                reject();
            }
        });
    });
}
exports.commandWithProgress = commandWithProgress;
function newError(text) {
    newSpinner(text).fail(text);
}
exports.newError = newError;
function newWarn(text) {
    newSpinner(text).warn(text);
}
exports.newWarn = newWarn;
function newSuccess(text) {
    newSpinner(text).succeed(text);
}
exports.newSuccess = newSuccess;
function newInfo(text) {
    newSpinner(text).info(text);
}
exports.newInfo = newInfo;
//# sourceMappingURL=commandWithProgress.js.map