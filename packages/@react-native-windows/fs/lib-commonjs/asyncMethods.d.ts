/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import fs from 'graceful-fs';
declare type PromisifedMethods = {
    access: typeof fs.access.__promisify__;
    appendFile: typeof fs.appendFile.__promisify__;
    chmod: typeof fs.chmod.__promisify__;
    chown: typeof fs.chown.__promisify__;
    copyFile: typeof fs.copyFile.__promisify__;
    lchmod: typeof fs.lchmod.__promisify__;
    lchown: typeof fs.lchown.__promisify__;
    link: typeof fs.link.__promisify__;
    lstat: typeof fs.lstat.__promisify__;
    mkdir: typeof fs.mkdir.__promisify__;
    mkdtemp: typeof fs.mkdtemp.__promisify__;
    open: typeof fs.open.__promisify__;
    opendir: typeof fs.opendir.__promisify__;
    readdir: typeof fs.readdir.__promisify__;
    readFile: typeof fs.readFile.__promisify__;
    readlink: typeof fs.readlink.__promisify__;
    realpath: typeof fs.realpath.__promisify__;
    rename: typeof fs.rename.__promisify__;
    rmdir: typeof fs.rmdir.__promisify__;
    stat: typeof fs.stat.__promisify__;
    symlink: typeof fs.symlink.__promisify__;
    truncate: typeof fs.truncate.__promisify__;
    unlink: typeof fs.unlink.__promisify__;
    utimes: typeof fs.utimes.__promisify__;
    writeFile: typeof fs.writeFile.__promisify__;
};
declare type ExtraMethods = {
    /**
     * Asynchronously tests whether or not the given path exists within the
     * file-system.
     * @param path A path to a file or directory.
     */
    exists: (path: fs.PathLike) => Promise<boolean>;
    /**
     * Asynchronously reads the contents of a file, then parses it as JSON.
     *
     * By default the parsed JSON file is assumed to be a JSON object, with no
     * assumptions made about content (`Record<string, unknown`). If the type is
     * known at call-time, it can be passed to this function to alter the return
     * type.This type is not validated to be correct at runtime.
     *
     * E.g. `await fs.readJsonFile<Document>(documentPath)`;
     *
     * @param path A path to a file.
     * @param options Optional set of options
     * @param options.encoding The encoding of the JSON file (defaults to utf-8)
     * @param options.flag File access flags
     */
    readJsonFile: <T = Record<string, unknown>>(path: fs.PathLike | number, options?: {
        encoding?: null | BufferEncoding;
        flag?: string | undefined;
    }) => Promise<T>;
};
export declare type AsyncMethods = PromisifedMethods & ExtraMethods;
declare const asyncMethods: AsyncMethods;
export default asyncMethods;
