"use strict";
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const runWindows_1 = require("../runWindows/runWindows");
test('getAnonymizedProjectName - Project Exists', async () => {
    const fooName = await runWindows_1.getAnonymizedProjectName(`${__dirname}/projects/FooPackage`);
    const barName = await runWindows_1.getAnonymizedProjectName(`${__dirname}/projects/BarPackage`);
    expect(typeof fooName).toBe('string');
    expect(typeof barName).toBe('string');
    expect(fooName.length).toBeGreaterThan(0);
    expect(barName.length).toBeGreaterThan(0);
    expect(fooName).not.toBe(barName);
});
test('getAnonymizedProjectName - Project Doesnt Exist', async () => {
    const emptyPackageName = await runWindows_1.getAnonymizedProjectName(`${__dirname}/projects/BlankApp`);
    expect(emptyPackageName).toBeNull();
});
//# sourceMappingURL=runWindows.test.js.map