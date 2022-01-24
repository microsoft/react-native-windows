/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import fs from 'graceful-fs';
declare const importedMethods: readonly ["accessSync", "appendFileSync", "chmodSync", "chownSync", "closeSync", "copyFileSync", "createReadStream", "createWriteStream", "existsSync", "fchmodSync", "fchownSync", "fdatasyncSync", "fstatSync", "fsyncSync", "ftruncateSync", "futimesSync", "lchmodSync", "lchownSync", "linkSync", "lstatSync", "mkdirSync", "mkdtempSync", "opendirSync", "openSync", "readdirSync", "readlinkSync", "readFileSync", "readSync", "realpathSync", "realpathSync", "renameSync", "rmdirSync", "statSync", "symlinkSync", "truncateSync", "unlinkSync", "utimesSync", "watch", "watchFile", "writeFileSync", "writeSync", "writeSync", "writevSync"];
declare type ValueOf<T extends readonly any[]> = T[number];
declare type ImportedMethodNames = ValueOf<typeof importedMethods>;
declare type ImportedMethods = Pick<typeof fs, ImportedMethodNames>;
declare type ExtraMethods = {
    /**
     * Synchronously reads the contents of a file, then parses it as JSON.
     *
     * By default the parsed JSON file is assumed to be a JSON object, with no
     * assumptions made about content (`Record<string, unknown`). If the type is
     * known at call-time, it can be passed to this function to alter the return
     * type.This type is not validated to be correct at runtime.
     *
     * E.g. `fs.readJsonFileSync<Document>(documentPath)`;
     *
     * @param path A path to a file.
     * @param options Optional set of options
     * @param options.encoding The encoding of the JSON file (defaults to utf-8)
     * @param options.flag File access flags
     */
    readJsonFileSync: <T = Record<string, unknown>>(path: fs.PathLike | number, options?: {
        encoding?: null | BufferEncoding;
        flag?: string | undefined;
    }) => T;
};
export declare type SyncMethods = ImportedMethods & ExtraMethods;
declare const syncMethods: SyncMethods;
export default syncMethods;
