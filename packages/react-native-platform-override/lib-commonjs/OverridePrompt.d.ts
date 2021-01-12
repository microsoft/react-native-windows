/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import Override from './Override';
import OverrideFactory from './OverrideFactory';
export declare type OverridePromptAnswers = {
    type: 'derived';
    baseFile: string;
    codeCopied: boolean;
    issue?: number;
} | {
    type: 'patch';
    baseFile: string;
    issue: number;
} | {
    type: 'platform';
} | {
    type: 'copy';
    baseFile: string;
    issue: number;
} | {
    type: 'directoryCopy';
    baseDirectory: string;
    issue: number;
};
export declare function overrideFromDetails(overridePath: string, answers: OverridePromptAnswers, factory: OverrideFactory): Promise<Override>;
export declare function promptForOverrideDetails(): Promise<OverridePromptAnswers>;
