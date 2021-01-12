"use strict";
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const path = require("path");
const dependencyConfig_1 = require("../config/dependencyConfig");
const generator_common_1 = require("../generator-common");
const templateRoot = path.resolve('../../../vnext/template');
const testProjectGuid = '{416476D5-974A-4EE2-8145-4E331297247E}';
async function tryMkdir(dir) {
    try {
        await fs.promises.mkdir(dir, { recursive: true });
    }
    catch (err) { }
}
function project(name, setup) {
    return [name, setup];
}
const projects = [
    // Nothing but a react-native.config.js, with nulls
    project('BlankLib'),
    // Nothing but a windows folder
    project('MissingProjectFilesLib', async (folder) => {
        const windowsDir = path.join(folder, 'windows');
        await tryMkdir(windowsDir);
    }),
    // New C++ project based on the template
    project('SimpleCppLib', async (folder) => {
        const windowsDir = path.join(folder, 'windows');
        await tryMkdir(windowsDir);
        const replacements = {
            name: 'SimpleCppLib',
            namespace: 'SimpleCppLib',
            useMustache: true,
            projectGuidUpper: testProjectGuid,
            projectGuidLower: testProjectGuid.toLowerCase(),
        };
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cpp-lib/proj/MyLib.sln'), path.join(windowsDir, 'SimpleCppLib.sln'), replacements, null);
        const projDir = path.join(windowsDir, 'SimpleCppLib');
        await tryMkdir(projDir);
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cpp-lib/proj/MyLib.vcxproj'), path.join(projDir, 'SimpleCppLib.vcxproj'), replacements, null);
    }),
    // New C# project based on the template
    project('SimpleCSharpLib', async (folder) => {
        const windowsDir = path.join(folder, 'windows');
        await tryMkdir(windowsDir);
        const replacements = {
            name: 'SimpleCSharpLib',
            namespace: 'SimpleCSharpLib',
            useMustache: true,
            projectGuidUpper: testProjectGuid,
            projectGuidLower: testProjectGuid.toLowerCase(),
        };
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cs-lib/proj/MyLib.sln'), path.join(windowsDir, 'SimpleCSharpLib.sln'), replacements, null);
        const projDir = path.join(windowsDir, 'SimpleCSharpLib');
        await tryMkdir(projDir);
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cs-lib/proj/MyLib.csproj'), path.join(projDir, 'SimpleCSharpLib.csproj'), replacements, null);
    }),
];
// Tests that given userConfig is null, the result will always be null
test.each(projects)('dependencyConfig - %s (userConfig is null)', async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);
    if (setup !== undefined) {
        await setup(folder);
    }
    const userConfig = null;
    const expectedConfig = null;
    expect(dependencyConfig_1.dependencyConfigWindows(folder, userConfig)).toBe(expectedConfig);
});
// Tests the result given a windows project config in react-native.config.js
test.each(projects)('dependencyConfig - %s (Use react-native.config.js)', async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);
    const rnc = require(path.join(folder, 'react-native.config.js'));
    if (setup !== undefined) {
        await setup(folder);
    }
    const userConfig = rnc.dependency.platforms.windows;
    const expectedConfig = rnc.expectedConfig;
    if (expectedConfig !== null) {
        expectedConfig.folder = folder;
    }
    expect(dependencyConfig_1.dependencyConfigWindows(folder, userConfig)).toEqual(expectedConfig);
});
// Tests the result of ignoring the windows project config in react-native.config.js
test.each(projects)('dependencyConfig - %s (Ignore react-native.config.js)', async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);
    const rnc = require(path.join(folder, 'react-native.config.js'));
    if (setup !== undefined) {
        await setup(folder);
    }
    const userConfig = {};
    const expectedConfig = rnc.expectedConfigIgnoringOverride;
    if (expectedConfig !== null) {
        expectedConfig.folder = folder;
    }
    expect(dependencyConfig_1.dependencyConfigWindows(folder, userConfig)).toEqual(expectedConfig);
});
//# sourceMappingURL=dependencyConfig.test.js.map