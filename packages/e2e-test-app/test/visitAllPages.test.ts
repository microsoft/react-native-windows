/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToApiExample, goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

afterEach(async () => {
  await verifyNoErrorLogs();
});

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

describe('visitAllPages', () => {
  for (const component of componentExamples) {
    if (
      component === 'Custom Native Accessibility Example' ||
      component === 'Moving Light Example' ||
      component === 'Drawing Island Example' ||
      component === 'Fabric Native Component' ||
      component === 'Fabric Native Component Yoga' ||
      component === 'Xaml WinUI3 (Experimental, for Fabric)'
    ) {
      continue; // disable fabric specific tests
    }

    test(component, async () => await goToComponentExample(component));
  }

  for (const api of apiExamples) {
    if (api === 'Transforms')
      // disable until either transformExample uses units, or that isn't an error
      continue;

    test(api, async () => {
      await goToApiExample(api);

      if (api === 'Invalid Props') {
        // InvalidPropsExample shows an expected error
        await verifyNoErrorLogs((errors: string[]) => {
          const expectedInvalidPropError =
            "Value '' for width is invalid. Cannot be converted to YGValue. Did you forget the %? Otherwise, simply use integer value.";
          expect(errors).toContain(expectedInvalidPropError);
          return errors.filter(err => err !== expectedInvalidPropError);
        });
      }
    });
  }
});
