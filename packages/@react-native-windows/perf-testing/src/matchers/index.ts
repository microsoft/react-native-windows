/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// Auto-register the toMatchPerfSnapshot matcher when this module is imported.
import './toMatchPerfSnapshot';

export {SnapshotManager} from './snapshotManager';
export type {SnapshotEntry, SnapshotFile} from './snapshotManager';
