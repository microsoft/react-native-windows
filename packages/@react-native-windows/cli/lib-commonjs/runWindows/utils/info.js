"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.getEnvironmentInfo = void 0;
// @ts-ignore no typings
const envinfo = require("envinfo");
async function getEnvironmentInfo() {
    return await envinfo.run({
        System: ['OS', 'CPU', 'Memory', 'Shell'],
        Binaries: ['Node', 'Yarn', 'npm', 'Watchman'],
        npmPackages: [
            'react',
            'react-native',
            'react-native-windows',
            '@react-native-community/cli',
            'react-native-cli',
        ],
        npmGlobalPackages: ['*react-native*'],
    });
}
exports.getEnvironmentInfo = getEnvironmentInfo;
//# sourceMappingURL=info.js.map