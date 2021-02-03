/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="webdriverio/webdriverio-core" />
import { RemoteOptions } from 'webdriverio';
export declare type EnvironmentOptions = {
    app?: string;
    winAppDriverBin?: string;
    webdriverOptions?: RemoteOptions;
};
