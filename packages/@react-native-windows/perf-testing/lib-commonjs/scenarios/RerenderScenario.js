"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.RerenderScenario = void 0;
const measurePerf_1 = require("../core/measurePerf");
/**
 * A reusable scenario that measures component re-render time.
 *
 * Accepts an optional `triggerRerender` callback to simulate
 * prop changes or state updates specific to the component.
 *
 * @example
 * ```typescript
 * const rerenderScenario = new RerenderScenario(
 *   'Text rerender',
 *   <Text testID="perf-text">Hello</Text>,
 *   {
 *     triggerRerender: (helpers) => {
 *       helpers.rerender(<Text testID="perf-text">Updated</Text>);
 *     },
 *   }
 * );
 * const metrics = await rerenderScenario.run();
 * ```
 */
class RerenderScenario {
    constructor(name, component, options = {}) {
        this.component = component;
        this.options = options;
        this.name = name;
        this.description = `Rerender scenario for ${name}`;
    }
    async run() {
        var _a, _b;
        return (0, measurePerf_1.measurePerf)(this.component, {
            name: this.name,
            runs: (_a = this.options.runs) !== null && _a !== void 0 ? _a : 10,
            warmupRuns: (_b = this.options.warmupRuns) !== null && _b !== void 0 ? _b : 1,
            scenario: async (helpers) => {
                if (this.options.triggerRerender) {
                    await this.options.triggerRerender(helpers);
                }
                else {
                    // Default: re-render the same component (forces reconciliation)
                    helpers.rerender(this.component);
                }
            },
        });
    }
}
exports.RerenderScenario = RerenderScenario;
//# sourceMappingURL=RerenderScenario.js.map