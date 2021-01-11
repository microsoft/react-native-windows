/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';
import * as simplegit from 'simple-git/promise';
import ActionQueue from './ActionQueue';
const REACT_NATIVE_GITHUB_URL = 'https://github.com/facebook/react-native.git';
const DEFAULT_DIR = path.join(os.tmpdir(), 'react-native-windows-override-git');
/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
export default class GitReactFileRepository {
    constructor() { }
    static createAndInit(gitDirectory) {
        return __awaiter(this, void 0, void 0, function* () {
            let repo = new GitReactFileRepository();
            repo.actionQueue = new ActionQueue();
            repo.gitDirectory = gitDirectory || DEFAULT_DIR;
            yield fs.promises.mkdir(repo.gitDirectory, { recursive: true });
            repo.gitClient = simplegit(repo.gitDirectory);
            repo.gitClient.silent(true);
            if (!(yield repo.gitClient.checkIsRepo())) {
                yield repo.gitClient.init();
                yield repo.gitClient.addRemote('origin', REACT_NATIVE_GITHUB_URL);
            }
            return repo;
        });
    }
    getFileContents(filename, reactNativeVersion) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.actionQueue.enqueue(() => __awaiter(this, void 0, void 0, function* () {
                yield this.checkoutVersion(reactNativeVersion);
                const filePath = path.join(this.gitDirectory, filename);
                try {
                    return (yield fs.promises.readFile(filePath)).toString();
                }
                catch (_a) {
                    return null;
                }
            }));
        });
    }
    /**
     * Generate a Git-style patch to transform the given file into the given
     * content.
     */
    generatePatch(filename, reactNativeVersion, newContent) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.actionQueue.enqueue(() => __awaiter(this, void 0, void 0, function* () {
                yield this.checkoutVersion(reactNativeVersion);
                const filePath = path.join(this.gitDirectory, filename);
                try {
                    yield fs.promises.writeFile(filePath, newContent);
                    const patch = yield this.gitClient.diff([
                        '--patch',
                        '--ignore-space-at-eol',
                    ]);
                    return patch;
                }
                finally {
                    yield this.gitClient.reset('hard');
                }
            }));
        });
    }
    /**
     * Apply a patch to the given file, returning the merged result, which may
     * include conflict markers. The underlying file is not mutated.
     */
    getPatchedFile(filename, reactNativeVersion, patchContent) {
        return __awaiter(this, void 0, void 0, function* () {
            return this.actionQueue.enqueue(() => __awaiter(this, void 0, void 0, function* () {
                yield this.checkoutVersion(reactNativeVersion);
                const filePath = path.join(this.gitDirectory, filename);
                const patchPath = path.join(this.gitDirectory, 'rnwgit.patch');
                try {
                    yield fs.promises.writeFile(patchPath, patchContent);
                    try {
                        yield this.gitClient.raw([
                            'apply',
                            '--3way',
                            '--whitespace=nowarn',
                            patchPath,
                        ]);
                    }
                    catch (ex) {
                        // Hack alert: simple-git doesn't populate exception information from
                        // conflicts when we're using raw commands (which we need to since it
                        // doesn't support apply). Try to detect if Git gave us a bad exit code
                        // because of merge conflicts, which we explicitly want to allow.
                        if (!ex.message.includes('with conflicts')) {
                            throw ex;
                        }
                    }
                    const patchedFile = yield fs.promises.readFile(filePath);
                    return patchedFile.toString();
                }
                finally {
                    yield this.gitClient.reset('hard');
                }
            }));
        });
    }
    checkoutVersion(reactNativeVersion) {
        return __awaiter(this, void 0, void 0, function* () {
            if (reactNativeVersion !== this.checkedOutVersion) {
                try {
                    yield this.gitClient.fetch('origin', `refs/tags/v${reactNativeVersion}`);
                    yield this.gitClient.checkout('FETCH_HEAD');
                    yield this.gitClient.reset('hard');
                    this.checkedOutVersion = reactNativeVersion;
                }
                catch (ex) {
                    throw new Error(`Failed to checkout tag 'v${reactNativeVersion}'. Does it exist?`);
                }
            }
        });
    }
}
//# sourceMappingURL=GitReactFileRepository.js.map