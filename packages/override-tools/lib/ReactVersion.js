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
import * as FileSearch from './FileSearch';
import * as fs from 'fs';
/**
 * Try to find the currently installed React Native version by searching for and
 * reading it's package.json.
 */
export function getInstalledRNVersion(searchPath) {
    return __awaiter(this, void 0, void 0, function* () {
        const packagePath = yield FileSearch.findReactPackage(searchPath);
        const packageJson = (yield fs.promises.readFile(packagePath)).toString();
        const version = JSON.parse(packageJson).version;
        if (typeof version !== 'string') {
            throw new Error('Unexpected formt of React Native package.json');
        }
        return version;
    });
}
//# sourceMappingURL=ReactVersion.js.map