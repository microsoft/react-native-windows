"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.usingRepository = exports.usingFiles = exports.usingGitReactRepo = exports.acquireGitRepo = exports.usingScratchDirectory = exports.acquireSratchDirectory = void 0;
const crypto = require("crypto");
const fs = require("fs");
const globby = require("globby");
const os = require("os");
const path = require("path");
const FileSystemRepository_1 = require("../FileSystemRepository");
const GitReactFileRepository_1 = require("../GitReactFileRepository");
const PackageUtils_1 = require("../PackageUtils");
/**
 * Helper to acquire a scratch directory which must be deleted using the
 * returned callback.
 */
async function acquireSratchDirectory() {
    const dir = path.join(os.tmpdir(), (await PackageUtils_1.getNpmPackage()).name, 'e2etest', crypto.randomBytes(16).toString('hex'));
    await fs.promises.mkdir(dir, { recursive: true });
    return [dir, async () => fs.promises.rmdir(dir, { recursive: true })];
}
exports.acquireSratchDirectory = acquireSratchDirectory;
/**
 * Helper to acquire a scratch directory which will be deleted after function
 * exit
 */
async function usingScratchDirectory(fn) {
    return usingAcquired(fn, await acquireSratchDirectory());
}
exports.usingScratchDirectory = usingScratchDirectory;
/**
 * Helper to acquire an isolated GitReactFileRepository whuch ust be deleted
 * using the returned callback.
 */
async function acquireGitRepo() {
    const [dir, dispose] = await acquireSratchDirectory();
    const gitReactRepo = await GitReactFileRepository_1.default.createAndInit(dir);
    return [gitReactRepo, dispose];
}
exports.acquireGitRepo = acquireGitRepo;
/**
 * Helper to acquire an isolated GitReactFileRepository which will be deleted
 * after function exit
 */
async function usingGitReactRepo(fn) {
    return usingAcquired(fn, await acquireGitRepo());
}
exports.usingGitReactRepo = usingGitReactRepo;
/**
 * Helper to create an isolated repository filled with specified files and
 * clean it up once execution finishes
 */
async function usingFiles(overridesToCopy, fn) {
    return await usingScratchDirectory(async (targetDirectory) => {
        const collateralPath = path.join(__dirname, 'collateral');
        await Promise.all(overridesToCopy.map(async (override) => {
            const src = path.join(collateralPath, override);
            const dst = path.join(targetDirectory, override);
            await fs.promises.mkdir(path.dirname(dst), { recursive: true });
            return await fs.promises.copyFile(src, dst);
        }));
        return await fn(new FileSystemRepository_1.default(targetDirectory), targetDirectory);
    });
}
exports.usingFiles = usingFiles;
/**
 * Helper to create an isolated repository cloned from a source folder and
 * clean it up once execution finishes
 */
async function usingRepository(sourceFolder, fn) {
    const collateralPath = path.join(__dirname, 'collateral');
    const srcRepo = path.join(collateralPath, sourceFolder);
    const srcFiles = (await globby(['**/*'], { cwd: srcRepo, absolute: true })).map(f => path.relative(collateralPath, f));
    return await usingFiles(srcFiles, async (repo, baseDir) => {
        return await fn(repo, path.join(baseDir, sourceFolder));
    });
}
exports.usingRepository = usingRepository;
/**
 * Pass an acquired resource to a function and dispose of it after function
 * exit
 */
async function usingAcquired(fn, acquired) {
    try {
        return await fn(acquired[0]);
    }
    finally {
        await acquired[1]();
    }
}
//# sourceMappingURL=Resource.js.map