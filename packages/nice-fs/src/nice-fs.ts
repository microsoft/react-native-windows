/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import asyncMethods, {AsyncMethods} from './asyncMethods';
import syncMethods, {SyncMethods} from './syncMethods';
import fs from 'graceful-fs';

export interface NiceFS extends AsyncMethods, SyncMethods {
  constants: typeof fs.constants;

  /**
   * `renameSync` should not be used, as it can cause EPERM issues on Windows
   * @deprecated
   */
  renameSync(oldPath: fs.PathLike, newPath: fs.PathLike): void;
}

const niceFs: NiceFS = {
  constants: fs.constants,
  ...asyncMethods,
  ...syncMethods,
};
export default niceFs;
