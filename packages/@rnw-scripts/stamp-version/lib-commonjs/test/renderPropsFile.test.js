"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const renderPropsFile_1 = __importDefault(require("../renderPropsFile"));
test('0.0.0-canary.100', async () => {
    expect(await (0, renderPropsFile_1.default)('0.0.0-canary.100')).toMatchSnapshot();
});
test('0.67.0-preview.1', async () => {
    expect(await (0, renderPropsFile_1.default)('0.67.0-preview.1')).toMatchSnapshot();
});
test('0.66.5', async () => {
    expect(await (0, renderPropsFile_1.default)('0.66.5')).toMatchSnapshot();
});
//# sourceMappingURL=renderPropsFile.test.js.map