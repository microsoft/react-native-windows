/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import * as ora from 'ora';
export declare function newSpinner(text: string): ora.Ora;
export declare function runPowerShellScriptFunction(taskDescription: string, script: string | null, funcName: string, verbose: boolean): Promise<void>;
export declare function commandWithProgress(spinner: ora.Ora, taskDoingName: string, command: string, args: string[], verbose: boolean): Promise<unknown>;
export declare function newError(text: string): void;
export declare function newWarn(text: string): void;
export declare function newSuccess(text: string): void;
export declare function newInfo(text: string): void;
