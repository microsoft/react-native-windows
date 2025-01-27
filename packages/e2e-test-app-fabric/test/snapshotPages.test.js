const React = require('react');
import {create, act} from 'react-test-renderer';
import {View} from 'react-native';
jest.useFakeTimers();

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

test('Component Control', () => {
  let tree;
  act(() => {
    tree = create(<View />);
  });
  expect(tree.toJSON()).toMatchSnapshot();
});

describe('snapshotAllPages', () => {
  for (const component of testerList.Components) {
    if (component.module.title == "New App Screen" || component.module.title == "Image"){
      // disable until errors are resolved
      continue;
    }

    test(component.module.title, () => {
      for (const example of component.module.examples) {
        // https://github.com/microsoft/react-native-windows/issues/13436
        if((component.module.title == 'FlatList' && (example.title == 'Basic' || example.title == "MultiColumn")) || (component.module.title == 'ScrollView' && example.title == '<ScrollView> RefreshControl\n') || (component.module.title == 'SectionList' && example.title == 'SectionList scrollable')){
          continue;
        }
        const Example = example.render;
        let tree;
        act(() => {
          tree = create(<Example />);
        });
        expect(tree.toJSON()).toMatchSnapshot();
      }
    });
  }

  for (const api of testerList.APIs) {
    if (api.module.title === 'Transforms' || api.module.title === 'TurboModule' || api.module.title === 'Cxx TurboModule' || 
    api.module.title === 'Appearance' || api.module.title === 'AccessibilityInfo' || api.module.title === 'Accessibility' || 
    api.module.title === 'Invalid Props' || api.module.title === 'Animated - Gratuitous App'){
      // disable while these modules have not been implemented yet
      continue;
    }

    test(api.module.title, () => {
      for (const example of api.module.examples) {
        const Example = example.render;
        let tree;
        act(() => {
          tree = create(<Example />);
        });
        expect(tree.toJSON()).toMatchSnapshot();
      }
    });
  }
});
