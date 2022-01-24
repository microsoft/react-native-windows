/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import { AsyncMethods } from './asyncMethods';
import { SyncMethods } from './syncMethods';
import fs from 'graceful-fs';
export interface ReactBativeWindowsFs extends AsyncMethods, SyncMethods {
    constants: typeof fs.constants;
    /**
     * `renameSync` should not be used, as it can cause EPERM issues on Windows
     * @deprecated
     */
    renameSync(oldPath: fs.PathLike, newPath: fs.PathLike): void;
}
declare const rnwFs: ReactBativeWindowsFs;
export default rnwFs;
