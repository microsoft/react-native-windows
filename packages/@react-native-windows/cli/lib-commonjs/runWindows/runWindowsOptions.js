"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.runWindowsOptions = void 0;
exports.runWindowsOptions = [
    {
        name: '--release',
        description: 'Specifies a release build',
    },
    {
        name: '--root [string]',
        description: 'Override the root directory for the windows build which contains the windows folder.',
        default: config => config.root,
    },
    {
        name: '--arch [string]',
        description: 'The build architecture (ARM, ARM64, x86, x64)',
        default: 'x86',
        parse: parseBuildArch,
    },
    {
        name: '--emulator',
        description: 'Deploys the app to an emulator',
    },
    {
        name: '--device',
        description: 'Deploys the app to a connected device',
    },
    {
        name: '--target [string]',
        description: 'Deploys the app to the specified GUID for a device.',
    },
    {
        name: '--remote-debugging',
        description: 'Deploys the app in remote debugging mode.',
    },
    {
        name: '--logging',
        description: 'Enables logging',
        default: false,
    },
    {
        name: '--no-packager',
        description: 'Do not launch packager while building',
        default: false,
    },
    {
        name: '--bundle',
        description: 'Enable Bundle configuration and it would be ReleaseBundle/DebugBundle other than Release/Debug',
        default: false,
    },
    {
        name: '--no-launch',
        description: 'Do not launch the app after deployment',
        default: false,
    },
    {
        name: '--no-autolink',
        description: 'Do not run autolinking',
        default: false,
    },
    {
        name: '--no-build',
        description: 'Do not build the solution',
        default: false,
    },
    {
        name: '--no-deploy',
        description: 'Do not deploy the app',
        default: false,
    },
    {
        name: '--sln [string]',
        description: "Override the app solution file determined by 'react-native config', e.g. windows\\myApp.sln",
        default: undefined,
    },
    {
        name: '--proj [string]',
        description: "Override the app project file determined by 'react-native config', e.g. windows\\myApp\\myApp.vcxproj",
        default: undefined,
    },
    {
        name: '--msbuildprops [string]',
        description: 'Comma separated props to pass to msbuild, eg: prop1=value1,prop2=value2',
    },
    {
        name: '--buildLogDirectory [string]',
        description: 'Optional directory where msbuild log files should be stored',
    },
    {
        name: '--info',
        description: 'Dump environment information',
        default: false,
    },
    {
        name: '--direct-debugging [number]',
        description: 'Enable direct debugging on specified port',
        parse: parseDirectDebuggingPort,
    },
    {
        name: '--no-telemetry',
        description: 'Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
        default: true,
    },
];
function parseBuildArch(arg) {
    const supportedArches = ['x86', 'x64', 'ARM64', 'ARM'];
    for (const supported of supportedArches) {
        if (arg.toLowerCase() === supported.toLowerCase()) {
            return supported;
        }
    }
    errorOut(`Unrecognized --arch '${arg}'. Expected one of ${supportedArches}`);
}
function parseDirectDebuggingPort(arg) {
    const num = parseInt(arg, 10);
    if (!Number.isInteger(num)) {
        errorOut(`Expected argument '--direct-debugging' to be a number`);
    }
    if (num < 1024 || num >= 65535) {
        errorOut('Direct debugging port it out of range');
    }
    return num;
}
function errorOut(arg) {
    console.error(arg);
    process.exit(1);
}
//# sourceMappingURL=runWindowsOptions.js.map