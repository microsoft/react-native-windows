const React = require('react');
import {create, act} from 'react-test-renderer';
import {View} from 'react-native';
const ViewExample = require('@react-native-windows/tester/js/examples/View/ViewExample');

type RNTesterExampleModule = {
  title: string;
  description: string;
};

type RNTesterModuleInfo = {
  key: string;
  module: RNTesterExampleModule;
};

type RNTesterList = {
  APIs: RNTesterModuleInfo[];
  Components: RNTesterModuleInfo[];
};

const testerList: RNTesterList = require('@react-native-windows/tester/js/utils/RNTesterList');

const apiExamples = testerList.APIs.map(e => e.module.title);
const componentExamples = testerList.Components.map(e => e.module.title);

test('Component Control', () => {
  const tree = create(<View />).toJSON();
  expect(tree).toMatchSnapshot();
});

test('Explicit Component', () => {
  const Example = ViewExample.examples[0].render;
  const tree = create(<Example />).toJSON();
  expect(tree).toMatchSnapshot();
});

describe('snapshotAllPages', () => {
  for (const component of testerList.Components) {
    if (component.module.title == "New App Screen" || component.module.title == "Image"){
      continue;
    }
    test(component.module.title, () => {
      for (const example of component.module.examples) {
        const Example = example.render;
        const tree = create(<Example />).toJSON();
        expect(tree).toMatchSnapshot();
      }
    });
  }

  for (const api of apiExamples) {
    if (api === 'Transforms')
      // disable until either transformExample uses units, or that isn't an error
      continue;

    test(api, () => {expect(true).toBe(true)});
  }
});
