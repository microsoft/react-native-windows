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
(async () => {
  const lock = new CrossProcessLock('TestLock');

  console.log('Trying to acquire lock');
  if (await lock.tryLock()) {
    console.log('Lock acquired!');
  } else {
    console.log('Failed to acqurie lock. Waiting...');
    await lock.lock();
    console.log('Lock acquired!');
  }

  setInterval(() => {}, 5000);
})();
