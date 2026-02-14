"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.MountScenario = void 0;
const measurePerf_1 = require("../core/measurePerf");
/**
 * A reusable scenario that measures component mount time.
 *
 * Use this when you need a standalone mount scenario outside of
 * ComponentPerfTestBase, or when building composite scenario pipelines.
 *
 * @example
 * ```typescript
 * const mountScenario = new MountScenario(
 *   'View mount',
 *   <View testID="perf-view" style={{ flex: 1 }} />,
 *   { runs: 20 }
 * );
 * const metrics = await mountScenario.run();
 * ```
 */
class MountScenario {
    constructor(name, component, options = {}) {
        this.component = component;
        this.options = options;
        this.name = name;
        this.description = `Mount scenario for ${name}`;
    }
    async run() {
        var _a, _b;
        return (0, measurePerf_1.measurePerf)(this.component, {
            name: this.name,
            runs: (_a = this.options.runs) !== null && _a !== void 0 ? _a : 10,
            warmupRuns: (_b = this.options.warmupRuns) !== null && _b !== void 0 ? _b : 1,
        });
    }
}
exports.MountScenario = MountScenario;
//# sourceMappingURL=MountScenario.js.map