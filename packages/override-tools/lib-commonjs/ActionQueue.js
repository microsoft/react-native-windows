"use strict";
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
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Executes actions sequentially, fullfilling the returned promise once complete.
 */
class ActionQueue {
    constructor() {
        this.actions = [];
    }
    enqueue(action) {
        return new Promise((resolve, reject) => {
            this.actions.push(() => __awaiter(this, void 0, void 0, function* () {
                try {
                    resolve(yield action());
                }
                catch (ex) {
                    reject(ex);
                }
            }));
            if (this.actions.length === 1) {
                this.pumpQueue();
            }
        });
    }
    pumpQueue() {
        return __awaiter(this, void 0, void 0, function* () {
            while (this.actions.length !== 0) {
                yield this.actions[0]();
                this.actions.shift();
            }
        });
    }
}
exports.default = ActionQueue;
//# sourceMappingURL=ActionQueue.js.map