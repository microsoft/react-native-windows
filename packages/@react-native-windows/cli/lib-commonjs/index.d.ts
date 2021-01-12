/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import { dependencyConfigWindows } from './config/dependencyConfig';
import { projectConfigWindows } from './config/projectConfig';
/**
 * Project generation options
 *
 *      _
 *     | |
 *   __| | __ _ _ __   __ _  ___ _ __
 *  / _` |/ _` | '_ \ / _` |/ _ \ '__|
 * | (_| | (_| | | | | (_| |  __/ |
 *  \__,_|\__,_|_| |_|\__, |\___|_|
 *                     __/ |
 *                    |___/
 *
 *
 * Properties on this interface must remain stable, as new versions of
 * react-native-windows-init may be used with local-cli dating back to 0.61.
 * All existing arguments must work correctly when changing this interface.
 */
export interface GenerateOptions {
    overwrite: boolean;
    language: 'cpp' | 'cs';
    experimentalNuGetDependency: boolean;
    nuGetTestVersion?: string;
    nuGetTestFeed?: string;
    useWinUI3: boolean;
    verbose: boolean;
    telemetry: boolean;
}
/**
 * Simple utility for running the Windows generator.
 *
 * @param  projectDir root project directory (i.e. contains index.js)
 * @param  name       name of the root JS module for this app
 * @param  ns         namespace for the project
 * @param  options    command line options container
 */
export declare function generateWindows(projectDir: string, name: string, ns: string, options: GenerateOptions): Promise<void>;
export declare const commands: import("@react-native-community/cli-types").Command<false>[];
export declare const dependencyConfig: typeof dependencyConfigWindows;
export declare const projectConfig: typeof projectConfigWindows;
