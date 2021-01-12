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
const fs = require("fs");
const path = require("path");
/**
 * Find the root directory of the repo upward from cwd
 */
exports.default = async () => {
    const root = await findUp(async (dir) => {
        const packagePath = path.join(dir, 'package.json');
        if (!(await findUp.exists(packagePath))) {
            return undefined;
        }
        const pkg = (await fs.promises.readFile(packagePath)).toString();
        return JSON.parse(pkg).name === 'react-native-windows-repo'
            ? dir
            : undefined;
    }, { type: 'directory' });
    if (!root) {
        throw new Error('Unable to find the root of react-native-windows. Are you running within the repo?');
    }
    return root;
};
//# sourceMappingURL=findRepoRoot.js.map