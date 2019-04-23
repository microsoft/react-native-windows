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
export default RNTesterList;
