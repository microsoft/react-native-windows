"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.UnmountScenario = void 0;
const measurePerf_1 = require("../core/measurePerf");
/**
 * A reusable scenario that measures component unmount time.
 *
 * @example
 * ```typescript
 * const unmountScenario = new UnmountScenario(
 *   'View unmount',
 *   <View testID="perf-view" style={{ flex: 1 }} />,
 * );
 * const metrics = await unmountScenario.run();
 * ```
 */
class UnmountScenario {
    constructor(name, component, options = {}) {
        this.component = component;
        this.options = options;
        this.name = name;
        this.description = `Unmount scenario for ${name}`;
    }
    async run() {
        var _a, _b;
        return (0, measurePerf_1.measurePerf)(this.component, {
            name: this.name,
            runs: (_a = this.options.runs) !== null && _a !== void 0 ? _a : 10,
            warmupRuns: (_b = this.options.warmupRuns) !== null && _b !== void 0 ? _b : 1,
            measureUnmount: true,
        });
    }
}
exports.UnmountScenario = UnmountScenario;
//# sourceMappingURL=UnmountScenario.js.map