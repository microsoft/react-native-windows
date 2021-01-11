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
import * as _ from 'lodash';
import * as path from 'path';
export class MockReactFileRepository {
    constructor(files) {
        this.files = files;
        this.files.forEach(file => (file.filename = path.normalize(file.filename)));
    }
    getFileContents(filename) {
        return __awaiter(this, void 0, void 0, function* () {
            const matches = _.filter(this.files, file => file.filename === filename);
            if (matches.length === 0) {
                return null;
            }
            else {
                return matches[0].content;
            }
        });
    }
    getVersion() {
        return '0.61.5';
    }
}
export class MockOverrideFileRepository {
    constructor(files) {
        this.files = files;
        this.files.forEach(file => (file.filename = path.normalize(file.filename)));
    }
    listFiles() {
        return __awaiter(this, void 0, void 0, function* () {
            return this.files.map(file => file.filename);
        });
    }
    getFileContents(filename) {
        return __awaiter(this, void 0, void 0, function* () {
            const matches = this.files.filter(file => file.filename === filename);
            if (matches.length === 0) {
                return null;
            }
            else {
                return matches[0].content;
            }
        });
    }
    setFileContents(filename, content) {
        return __awaiter(this, void 0, void 0, function* () {
            const matchFile = this.files.find(file => file.filename === filename);
            matchFile.content = content;
        });
    }
}
//# sourceMappingURL=MockFileRepository.js.map