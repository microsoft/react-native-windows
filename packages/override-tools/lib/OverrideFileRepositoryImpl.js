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
const DEFAULT_FILTER = /^.*\.(js|ts|jsx|tsx|cpp|h)$/;
/**
 * Allows reading phsyical override files based on a passed in directory
 */
export default class OverrideFileRepositoryImpl {
    constructor(baseDir, filter) {
        this.baseDir = baseDir;
        this.filter = filter || DEFAULT_FILTER;
    }
    listFiles() {
        return __awaiter(this, void 0, void 0, function* () {
            return (yield this.listFilesRec(this.baseDir))
                .filter(file => this.filter.test(file))
                .map(file => path.relative(this.baseDir, file));
        });
    }
    getFileContents(filename) {
        return __awaiter(this, void 0, void 0, function* () {
            const filePath = path.join(this.baseDir, filename);
            try {
                return (yield fs.promises.readFile(filePath)).toString();
            }
            catch (_a) {
                return null;
            }
        });
    }
    setFileContents(filename, content) {
        return __awaiter(this, void 0, void 0, function* () {
            const filePath = path.join(this.baseDir, filename);
            return fs.promises.writeFile(filePath, content);
        });
    }
    listFilesRec(file) {
        return __awaiter(this, void 0, void 0, function* () {
            const stat = yield fs.promises.stat(file);
            if (!stat.isDirectory()) {
                return [file];
            }
            const ret = [];
            const subfiles = yield fs.promises.readdir(file);
            yield Promise.all(subfiles.map((subfile) => __awaiter(this, void 0, void 0, function* () {
                const fullPath = path.join(file, subfile);
                const childFiles = yield this.listFilesRec(fullPath);
                ret.push(...childFiles);
            })));
            return ret;
        });
    }
}
//# sourceMappingURL=OverrideFileRepositoryImpl.js.map