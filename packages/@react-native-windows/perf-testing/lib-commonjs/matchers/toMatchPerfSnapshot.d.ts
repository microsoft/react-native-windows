/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import type { PerfThreshold } from '../interfaces/PerfThreshold';
/**
 * Extend Jest's Matchers interface so `toMatchPerfSnapshot` is recognized.
 */
declare global {
    namespace jest {
        interface Matchers<R> {
            toMatchPerfSnapshot(threshold?: Partial<PerfThreshold>): R;
        }
    }
}
