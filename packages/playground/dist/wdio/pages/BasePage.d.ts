/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
/// <reference types="@wdio/sync/webdriverio-core" />
export declare function By(testId: string): WebdriverIO.Element;
export declare function wait(timeout: number): Promise<unknown>;
export declare class BasePage {
    isPageLoaded(): boolean;
    waitForPageLoaded(timeout?: number): void;
    protected timeoutForPageLoaded(currentTimeout?: number): number;
    protected get loadButton(): WebdriverIO.Element;
    private get reactControlErrorMessage();
    private isPagedLoadedOrLoadBundleError;
    private waitforPageTimeout;
}
