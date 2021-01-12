"use strict";
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const path = require("path");
const projectConfig_1 = require("../config/projectConfig");
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
    project('BlankApp'),
    // Nothing but a windows folder
    project('MissingProjectFilesApp', async (folder) => {
        const windowsDir = path.join(folder, 'windows');
        await tryMkdir(windowsDir);
    }),
    // New C++ project based on the template
    project('SimpleCppApp', async (folder) => {
        const windowsDir = path.join(folder, 'windows');
        await tryMkdir(windowsDir);
        const replacements = {
            name: 'SimpleCppApp',
            namespace: 'SimpleCppApp',
            useMustache: true,
            projectGuidUpper: testProjectGuid,
            projectGuidLower: testProjectGuid.toLowerCase(),
        };
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cpp-app/proj/MyApp.sln'), path.join(windowsDir, 'SimpleCppApp.sln'), replacements, null);
        const projDir = path.join(windowsDir, 'SimpleCppApp');
        await tryMkdir(projDir);
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cpp-app/proj/MyApp.vcxproj'), path.join(projDir, 'SimpleCppApp.vcxproj'), replacements, null);
    }),
    // New C# project based on the template
    project('SimpleCSharpApp', async (folder) => {
        const windowsDir = path.join(folder, 'windows');
        await tryMkdir(windowsDir);
        const replacements = {
            name: 'SimpleCSharpApp',
            namespace: 'SimpleCSharpApp',
            useMustache: true,
            projectGuidUpper: testProjectGuid,
            projectGuidLower: testProjectGuid.toLowerCase(),
        };
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cs-app/proj/MyApp.sln'), path.join(windowsDir, 'SimpleCSharpApp.sln'), replacements, null);
        const projDir = path.join(windowsDir, 'SimpleCSharpApp');
        await tryMkdir(projDir);
        await generator_common_1.copyAndReplace(path.join(templateRoot, 'cs-app/proj/MyApp.csproj'), path.join(projDir, 'SimpleCSharpApp.csproj'), replacements, null);
    }),
];
// Tests that given userConfig is null, the result will always be null
test.each(projects)('projectConfig - %s (userConfig is null)', async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);
    if (setup !== undefined) {
        await setup(folder);
    }
    const userConfig = null;
    const expectedConfig = null;
    expect(projectConfig_1.projectConfigWindows(folder, userConfig)).toBe(expectedConfig);
});
// Tests the result given a windows project config in react-native.config.js
test.each(projects)('projectConfig - %s (Use react-native.config.js)', async (name, setup) => {
    const folder = path.resolve('src/e2etest/projects/', name);
    const rnc = require(path.join(folder, 'react-native.config.js'));
    if (setup !== undefined) {
        await setup(folder);
    }
    const userConfig = rnc.project.windows;
    const expectedConfig = rnc.expectedConfig;
    if (expectedConfig !== null) {
        expectedConfig.folder = folder;
    }
    expect(projectConfig_1.projectConfigWindows(folder, userConfig)).toEqual(expectedConfig);
});
// Tests the result of ignoring the windows project config in react-native.config.js
test.each(projects)('projectConfig - %s (Ignore react-native.config.js)', async (name, setup) => {
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
    expect(projectConfig_1.projectConfigWindows(folder, userConfig)).toEqual(expectedConfig);
});
//# sourceMappingURL=projectConfig.test.js.map