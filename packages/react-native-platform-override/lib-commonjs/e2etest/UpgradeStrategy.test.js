"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __rest = (this && this.__rest) || function (s, e) {
    var t = {};
    for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p) && e.indexOf(p) < 0)
        t[p] = s[p];
    if (s != null && typeof Object.getOwnPropertySymbols === "function")
        for (var i = 0, p = Object.getOwnPropertySymbols(s); i < p.length; i++) {
            if (e.indexOf(p[i]) < 0 && Object.prototype.propertyIsEnumerable.call(s, p[i]))
                t[p[i]] = s[p[i]];
        }
    return t;
};
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const path = require("path");
const UpgradeStrategy_1 = require("../UpgradeStrategy");
const Resource_1 = require("./Resource");
const Hash_1 = require("../Hash");
const isutf8_1 = require("isutf8");
let gitReactRepo;
let disposeReactRepo;
beforeAll(async () => {
    [gitReactRepo, disposeReactRepo] = await Resource_1.acquireGitRepo();
});
afterAll(async () => {
    await disposeReactRepo();
});
test('assumeUpToDate', async () => {
    const overrideFile = '0.61.5/flowconfig.windows.conflict';
    const strategy = UpgradeStrategy_1.UpgradeStrategies.assumeUpToDate(overrideFile);
    await evaluateStrategy({
        strategy,
        overrideFile,
        upgradeVersion: '0.62.2',
        allowConflicts: true,
        expected: {
            referenceFile: '0.61.5/flowconfig.windows.conflict',
            overrideName: overrideFile,
            filesWritten: false,
            hasConflicts: false,
        },
    });
});
test('threeWayMerge - Simple Addition', async () => {
    const overrideFile = '0.61.5/flowconfig.windows.addition';
    const baseFile = '.flowconfig';
    const baseVersion = '0.61.5';
    const strategy = UpgradeStrategy_1.UpgradeStrategies.threeWayMerge(overrideFile, baseFile, baseVersion);
    await evaluateStrategy({
        strategy,
        overrideFile,
        upgradeVersion: '0.62.2',
        allowConflicts: true,
        expected: {
            referenceFile: '0.62.2/flowconfig.windows.addition',
            overrideName: overrideFile,
            filesWritten: true,
            hasConflicts: false,
        },
    });
});
test('threeWayMerge - Text Conflict (Allowed)', async () => {
    const overrideFile = '0.61.5/flowconfig.windows.conflict';
    const baseFile = '.flowconfig';
    const baseVersion = '0.61.5';
    const strategy = UpgradeStrategy_1.UpgradeStrategies.threeWayMerge(overrideFile, baseFile, baseVersion);
    await evaluateStrategy({
        strategy,
        overrideFile,
        upgradeVersion: '0.62.2',
        allowConflicts: true,
        expected: {
            referenceFile: '0.62.2/flowconfig.windows.conflict',
            overrideName: overrideFile,
            filesWritten: true,
            hasConflicts: true,
        },
    });
});
test('threeWayMerge - Text Conflict (Disallowed)', async () => {
    const overrideFile = '0.61.5/flowconfig.windows.conflict';
    const baseFile = '.flowconfig';
    const baseVersion = '0.61.5';
    const strategy = UpgradeStrategy_1.UpgradeStrategies.threeWayMerge(overrideFile, baseFile, baseVersion);
    await evaluateStrategy({
        strategy,
        overrideFile,
        upgradeVersion: '0.62.2',
        allowConflicts: false,
        expected: {
            referenceFile: '0.61.5/flowconfig.windows.conflict',
            overrideName: overrideFile,
            filesWritten: false,
            hasConflicts: true,
        },
    });
});
test('threeWayMerge - Binary Conflict', async () => {
    const overrideFile = '0.59.9/Icon-60@2x.conflict.png';
    const baseFile = 'RNTester/RNTester/Images.xcassets/AppIcon.appiconset/Icon-60@2x.png';
    const baseVersion = '0.59.9';
    const strategy = UpgradeStrategy_1.UpgradeStrategies.threeWayMerge(overrideFile, baseFile, baseVersion);
    await evaluateStrategy({
        strategy,
        overrideFile,
        upgradeVersion: '0.62.2',
        allowConflicts: true,
        expected: {
            referenceFile: '0.59.9/Icon-60@2x.conflict.png',
            overrideName: overrideFile,
            filesWritten: false,
            hasConflicts: true,
        },
    });
});
test('copyFile', async () => {
    const overrideFile = '0.61.5/flowconfig.windows.conflict';
    const baseFile = '.flowconfig';
    const strategy = UpgradeStrategy_1.UpgradeStrategies.copyFile(overrideFile, baseFile);
    await evaluateStrategy({
        strategy,
        overrideFile,
        upgradeVersion: '0.62.2',
        allowConflicts: true,
        expected: {
            referenceFile: '0.62.2/flowconfig.pristine',
            overrideName: overrideFile,
            filesWritten: true,
            hasConflicts: false,
        },
    });
});
test('copyDirectory - New Files', async () => {
    const overrideDir = 'src/bots';
    const baseDir = 'bots';
    const strategy = UpgradeStrategy_1.UpgradeStrategies.copyDirectory(overrideDir, baseDir);
    await Resource_1.usingFiles([], async (overrideRepo) => {
        const results = await strategy.upgrade(gitReactRepo, overrideRepo, '0.62.2', true);
        expect(results).toEqual({
            overrideName: overrideDir,
            filesWritten: true,
            hasConflicts: false,
        });
        expect((await overrideRepo.listFiles()).sort()).toEqual([
            'src/bots/.babelrc',
            'src/bots/README.md',
            'src/bots/code-analysis-bot.js',
            'src/bots/dangerfile.js',
            'src/bots/package.json',
            'src/bots/yarn.lock',
        ]);
    });
});
test('copyDirectory - New Content', async () => {
    const overrideFiles = [
        '0.62.2/bots/.babelrc',
        '0.62.2/bots/README.md',
        '0.62.2/bots/code-analysis-bot.js',
        '0.62.2/bots/dangerfile.js',
        '0.62.2/bots/package.json',
        '0.62.2/bots/yarn.lock',
    ];
    const strategy = UpgradeStrategy_1.UpgradeStrategies.copyDirectory('0.62.2/bots', 'bots');
    await Resource_1.usingFiles(overrideFiles, async (overrideRepo) => {
        const correctHash = await Hash_1.hashFileOrDirectory(overrideFiles[0], overrideRepo);
        expect(correctHash).not.toBeNull();
        await overrideRepo.writeFile(overrideFiles[0], 'Garbage Data');
        expect(await Hash_1.hashFileOrDirectory(overrideFiles[0], overrideRepo)).not.toEqual(correctHash);
        const results = await strategy.upgrade(gitReactRepo, overrideRepo, '0.62.2', true);
        expect(results).toEqual({
            overrideName: '0.62.2/bots',
            filesWritten: true,
            hasConflicts: false,
        });
        expect(await Hash_1.hashFileOrDirectory(overrideFiles[0], overrideRepo)).toEqual(correctHash);
    });
});
test('copyDirectory - Deleted Content', async () => {
    const overrideFiles = [
        '0.62.2/bots/.babelrc',
        '0.62.2/bots/README.md',
        '0.62.2/bots/code-analysis-bot.js',
        '0.62.2/bots/dangerfile.js',
        '0.62.2/bots/package.json',
        '0.62.2/bots/yarn.lock',
    ];
    const strategy = UpgradeStrategy_1.UpgradeStrategies.copyDirectory('0.62.2/bots', 'bots');
    await Resource_1.usingFiles(overrideFiles, async (overrideRepo) => {
        await overrideRepo.writeFile('0.62.2/bots/extraFile.txt', 'Delete me');
        expect(await overrideRepo.stat('0.62.2/bots/extraFile.txt')).toBe('file');
        const results = await strategy.upgrade(gitReactRepo, overrideRepo, '0.62.2', true);
        expect(results).toEqual({
            overrideName: '0.62.2/bots',
            filesWritten: true,
            hasConflicts: false,
        });
        expect(await overrideRepo.stat('0.62.2/bots/extraFile.txt')).toBe('none');
    });
});
test('copyDirectory - Preserves Line Endings', async () => {
    const overrideFiles = [
        '0.62.2/bots/.babelrc',
        '0.62.2/bots/README.md',
        '0.62.2/bots/code-analysis-bot.js',
        '0.62.2/bots/dangerfile.js',
        '0.62.2/bots/package.json',
        '0.62.2/bots/yarn.lock',
    ];
    const strategy = UpgradeStrategy_1.UpgradeStrategies.copyDirectory('0.62.2/bots', 'bots');
    await Resource_1.usingFiles(overrideFiles, async (overrideRepo) => {
        const origContent = (await overrideRepo.readFile(overrideFiles[1])).toString();
        const switchedEndings = origContent.includes('\r\n')
            ? origContent.replace(/\r\n/g, '\n')
            : origContent.replace(/(?<!\r)\n/g, '\r\n');
        expect(origContent).not.toEqual(switchedEndings);
        await overrideRepo.writeFile(overrideFiles[1], switchedEndings);
        const results = await strategy.upgrade(gitReactRepo, overrideRepo, '0.62.2', true);
        expect(results).toEqual({
            overrideName: '0.62.2/bots',
            filesWritten: true,
            hasConflicts: false,
        });
        expect((await overrideRepo.readFile(overrideFiles[1])).toString()).toEqual(switchedEndings);
    });
});
async function evaluateStrategy(opts) {
    await Resource_1.usingFiles([opts.overrideFile], async (overrideRepo, overridesPath) => {
        const actualResult = await opts.strategy.upgrade(gitReactRepo, overrideRepo, opts.upgradeVersion, opts.allowConflicts);
        const _a = opts.expected, { referenceFile } = _a, expectedResult = __rest(_a, ["referenceFile"]);
        expect(actualResult).toEqual(expectedResult);
        const actualContent = await fs.promises.readFile(path.join(overridesPath, opts.overrideFile));
        const expectedContent = await fs.promises.readFile(path.join(__dirname, 'collateral', referenceFile));
        if (isutf8_1.default(actualContent) && isutf8_1.default(expectedContent)) {
            expect(actualContent.toString()).toBe(expectedContent.toString());
        }
        else {
            expect(actualContent.compare(expectedContent)).toBe(0);
        }
    });
}
//# sourceMappingURL=UpgradeStrategy.test.js.map