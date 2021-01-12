/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import * as appInsights from 'applicationinsights';
export declare class Telemetry {
    static client?: appInsights.TelemetryClient | undefined;
    static disable(): void;
    static setup(): void;
    static isCI(): boolean;
    static shouldDisable: boolean;
}
/**
 * Sanitize any paths that appear between quotes (''), brackets ([]), or double quotes ("").
 * @param msg the string to sanitize
 */
export declare function sanitizeMessage(msg: string): string;
export declare function sanitizeFrame(frame: any): void;
export declare function tryGetErrorCode(msg: string): string | undefined;
/**
 * Remove PII from exceptions' stack traces and messages
 * @param envelope the telemetry envelope. Provided by AppInsights.
 */
export declare function sanitizeEnvelope(envelope: any): boolean;
export declare function isMSFTInternal(): boolean;
export declare function getDiskFreeSpace(drivePath: string | null): number;
