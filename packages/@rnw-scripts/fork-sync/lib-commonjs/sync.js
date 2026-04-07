"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Wrapper script for fork-sync that points to the react-native-windows root.
 *
 * Usage:
 *   node packages/@rnw-scripts/fork-sync/lib-commonjs/sync.js --dep fmt
 *   node packages/@rnw-scripts/fork-sync/lib-commonjs/sync.js --dep fmt --status
 *   node packages/@rnw-scripts/fork-sync/lib-commonjs/sync.js --dep fmt --continue
 *
 * All arguments are forwarded to fork-sync with `-C` prepended
 * so it finds the sync-manifest.json in the repository root.
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
const child_process_1 = require("child_process");
const path = __importStar(require("path"));
const syncScript = path.join(__dirname, '..', 'node_modules', '@rnx-kit', 'fork-sync', 'lib', 'sync.js');
const repoRoot = path.join(__dirname, '..', '..', '..', '..');
const args = [syncScript, '-C', repoRoot, ...process.argv.slice(2)];
try {
    (0, child_process_1.execFileSync)('node', args, { stdio: 'inherit' });
}
catch (e) {
    // execFileSync throws on non-zero exit; stdio is already inherited
    const err = e;
    process.exit(err.status ?? 1);
}
//# sourceMappingURL=sync.js.map