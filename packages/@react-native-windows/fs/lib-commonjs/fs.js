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
const asyncMethods_1 = __importDefault(require("./asyncMethods"));
const syncMethods_1 = __importDefault(require("./syncMethods"));
const graceful_fs_1 = __importDefault(require("graceful-fs"));
const rnwFs = {
    constants: graceful_fs_1.default.constants,
    ...asyncMethods_1.default,
    ...syncMethods_1.default,
};
exports.default = rnwFs;
//# sourceMappingURL=fs.js.map