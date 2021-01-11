var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import CrossProcessLock from '../CrossProcessLock';
/**
 * CrossProcessLock cannot easily be unit tested. In the absence of automated
 * tests, this utility program can be used for simple manual testing.
 */
(() => __awaiter(void 0, void 0, void 0, function* () {
    const lock = new CrossProcessLock('TestLock');
    console.log('Trying to acquire lock');
    if (yield lock.tryLock()) {
        console.log('Lock acquired!');
    }
    else {
        console.log('Failed to acqurie lock. Waiting...');
        yield lock.lock();
        console.log('Lock acquired!');
    }
    setInterval(() => { }, 5000);
}))();
//# sourceMappingURL=testLocks.js.map