"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const DiffStrategy_1 = require("../DiffStrategy");
const Resource_1 = require("./Resource");
let gitReactRepo;
let disposeReactRepo;
beforeAll(async () => {
    [gitReactRepo, disposeReactRepo] = await Resource_1.acquireGitRepo();
});
afterAll(async () => {
    await disposeReactRepo();
});
test('assumeSame', async () => {
    const overrideFile = '0.62.2/flowconfig.pristine';
    const diff = await evaluateStrategy(DiffStrategy_1.DiffStrategies.asssumeSame(), overrideFile);
    expect(diff).toBe('');
});
test('compareBaseFile - No Difference', async () => {
    const overrideFile = '0.62.2/flowconfig.pristine';
    const diff = await evaluateStrategy(DiffStrategy_1.DiffStrategies.compareBaseFile(overrideFile, '.flowconfig', '0.62.2'), overrideFile);
    expect(diff).toBe('');
});
test('compareBaseFile - Difference', async () => {
    const overrideFile = '0.62.2/flowconfig.windows.addition';
    const diff = await evaluateStrategy(DiffStrategy_1.DiffStrategies.compareBaseFile(overrideFile, '.flowconfig', '0.62.2'), overrideFile);
    expect(diff).toBe(`@@ -15,6 +15,8 @@
 ; require from fbjs/lib instead: require('fbjs/lib/warning')
 .*/node_modules/warning/.*
 
+; This change is Windows only
+
 ; Flow doesn't support platforms
 .*/Libraries/Utilities/LoadingView.js`);
});
async function evaluateStrategy(strategy, overrideFile) {
    return Resource_1.usingFiles([overrideFile], async (overrideRepo) => {
        return strategy.diff(gitReactRepo, overrideRepo);
    });
}
//# sourceMappingURL=DiffStrategy.test.js.map