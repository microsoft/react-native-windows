/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
import * as fs from 'fs';
import * as path from 'path';
/**
 * Try to find the path to React Native's package.json
 */
export function findReactPackage(targetPath) {
    return __awaiter(this, void 0, void 0, function* () {
        const relativeRnPackage = 'node_modules\\react-native\\package.json';
        return findFileAbove(targetPath, relativeRnPackage);
    });
}
/**
 * Search for a manifest above an override file and return its path
 */
export function findManifest(overridePath) {
    return __awaiter(this, void 0, void 0, function* () {
        return findFileAbove(overridePath, 'overrides.json');
    });
}
/**
 * Search for the target path segment in all directories above the base
 */
function findFileAbove(base, target) {
    return __awaiter(this, void 0, void 0, function* () {
        let searchPath = path.resolve(base);
        while (path.resolve(searchPath, '..') !== searchPath) {
            try {
                const fullPath = path.join(searchPath, target);
                yield fs.promises.access(fullPath);
                return fullPath;
            }
            catch (ex) {
                if (ex.code === 'ENOENT') {
                    searchPath = path.resolve(searchPath, '..');
                }
                else {
                    throw ex;
                }
            }
        }
        throw new Error(`Could not find file '${target}'`);
    });
}
//# sourceMappingURL=FileSearch.js.map