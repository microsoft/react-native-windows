/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import { GenerateOptions } from '..';
export declare function copyProjectTemplateAndReplace(srcRootPath: string, destPath: string, newProjectName: string, namespace: string, options: GenerateOptions): Promise<void>;
export declare function installDependencies(options: {
    verbose: boolean;
}): void;
