/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
/**
 * Text to replace, + config options
 */
export declare type Replacements = {
    useMustache?: boolean;
    regExpPatternsToRemove?: RegExp[];
    [key: string]: any;
};
/**
 * Get a source file and replace parts of its contents.
 * @param srcPath Path to the source file.
 * @param replacements e.g. {'TextToBeReplaced': 'Replacement'}
 * @return The contents of the file with the replacements applied.
 */
export declare function resolveContents(srcPath: string, replacements: Replacements): string;
export declare function createDir(destPath: string): void;
export declare function copyAndReplaceWithChangedCallback(srcPath: string, destRoot: string, relativeDestPath: string, replacements?: Record<string, string>, alwaysOverwrite?: boolean): Promise<void>;
export declare function copyAndReplaceAll(srcPath: string, destPath: string, relativeDestDir: string, replacements: Replacements, alwaysOverwrite: boolean): Promise<void>;
