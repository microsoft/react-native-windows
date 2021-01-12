"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const PackageUtils_1 = require("../PackageUtils");
test('getNpmPackage - Sanity Test', async () => {
    const npmPackage = await PackageUtils_1.getNpmPackage();
    expect(npmPackage.name).toBe('react-native-platform-override');
});
//# sourceMappingURL=PackageUtils.test.js.map