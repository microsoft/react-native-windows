/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import React = require('react');
interface IRNTesterExample {
    key: string;
    module: IRNTesterModule;
}
interface IRNTesterModule {
    title: string;
    description: string;
    external: boolean;
    examples: [IRNTesterModuleExample];
}
interface IRNTesterModuleExample {
    title: string;
    description: string;
    render(): React.Component;
}
declare const RNTesterList: {
    APIExamples: IRNTesterExample[];
    ComponentExamples: IRNTesterExample[];
    Modules: {
        [key: string]: IRNTesterModule;
    };
};
export = RNTesterList;
//# sourceMappingURL=RNTesterList.windows.d.ts.map