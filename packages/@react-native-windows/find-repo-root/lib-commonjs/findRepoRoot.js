"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */
Object.defineProperty(exports, "__esModule", { value: true });
const findUp = require("find-up");
const path = require("path");
/**
 * Find the root directory of a repo upward from cwd
 */
exports.default = async () => {
    const root = await findUp(async (dir) => {
        return (await findUp.exists(path.join(dir, '.git'))) ? dir : undefined;
    }, { type: 'directory' });
    if (!root) {
        throw new Error('Unable to find the root of react-native-windows. Are you running within the repo?');
    }
    return root;
};
//# sourceMappingURL=findRepoRoot.js.map