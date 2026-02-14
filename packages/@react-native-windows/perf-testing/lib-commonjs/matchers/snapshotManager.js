"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.SnapshotManager = void 0;
const fs = __importStar(require("fs"));
const path = __importStar(require("path"));
/**
 * Manages reading and writing of perf snapshot files.
 *
 * Snapshot files live in `__perf_snapshots__/` adjacent to the test file
 * and are committed to git, mirroring Jest's visual snapshot pattern.
 */
class SnapshotManager {
    /**
     * Resolve the snapshot directory and file path for a given test file.
     */
    static getSnapshotPath(testFilePath) {
        const testDir = path.dirname(testFilePath);
        const snapshotDir = path.join(testDir, '__perf_snapshots__');
        const snapshotFile = path.join(snapshotDir, `${path.basename(testFilePath)}.perf-baseline.json`);
        return { dir: snapshotDir, file: snapshotFile };
    }
    /**
     * Load an existing snapshot file, or return an empty object.
     */
    static load(snapshotFilePath) {
        if (fs.existsSync(snapshotFilePath)) {
            const content = fs.readFileSync(snapshotFilePath, 'utf-8');
            const parsed = JSON.parse(content);
            // JSON.stringify turns Infinity into null — restore it on load
            for (const entry of Object.values(parsed)) {
                if (entry.threshold && entry.threshold.maxDuration === null) {
                    entry.threshold.maxDuration = Infinity;
                }
            }
            return parsed;
        }
        return {};
    }
    /**
     * Save a snapshot file to disk.
     * Creates the directory if it doesn't exist.
     */
    static save(snapshotFilePath, snapshots) {
        const dir = path.dirname(snapshotFilePath);
        if (!fs.existsSync(dir)) {
            fs.mkdirSync(dir, { recursive: true });
        }
        fs.writeFileSync(snapshotFilePath, JSON.stringify(snapshots, null, 2) + '\n', 'utf-8');
    }
    /**
     * Build a snapshot key from the test name.
     * Follows Jest's `"test name 1"` convention.
     */
    static buildKey(testName) {
        return `${testName} 1`;
    }
}
exports.SnapshotManager = SnapshotManager;
//# sourceMappingURL=snapshotManager.js.map