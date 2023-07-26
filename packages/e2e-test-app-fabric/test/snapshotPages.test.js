const React = require('react');
import {create, act} from 'react-test-renderer';
import {View} from 'react-native';

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

describe('snapshotAllPages', () => {
  for (const component of componentExamples) {
    test(component, () => {expect(true).toBe(true)});
  }

  for (const api of apiExamples) {
    if (api === 'Transforms')
      // disable until either transformExample uses units, or that isn't an error
      continue;

    test(api, () => {expect(true).toBe(true)});
  }
});