/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
declare class DeviceInfo {
    readonly name: string;
    readonly guid: string;
    readonly ip: string;
    private index;
    private type;
    constructor(name: string, guid: string, ip: string, index: number, type: string);
    toString(): string;
}
export default class WinAppDeployTool {
    private path;
    constructor();
    get isAvailable(): boolean;
    findDevice(target: string): DeviceInfo;
    enumerateDevices(): DeviceInfo[];
    installAppPackage(pathToAppxPackage: string, targetDevice: DeviceInfo, shouldLaunch: boolean, shouldUpdate: boolean, pin: boolean, verbose?: boolean): Promise<void>;
    uninstallAppPackage(appName: string, targetDevice: DeviceInfo, verbose: boolean): Promise<void>;
}
export {};
