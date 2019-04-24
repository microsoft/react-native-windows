// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import RNTesterList from './RNTesterList.uwp';
import { RNTesterAction } from './RNTesterActions';

interface IRNTesterNavigationState {
  openExample?: string;
}

function rnTesterNavigationReducer(
  state: IRNTesterNavigationState | undefined,
  action: RNTesterAction
): IRNTesterNavigationState {
  if (
    // Default value is to see example list
    !state ||
    // Handle the explicit list action
    action.type === 'RNTesterListAction' ||
    // Handle requests to go back to the list when an example is open
    (state.openExample && action.type === 'RNTesterBackAction')
  ) {
    return {
      // A null openExample will cause the views to display the RNTester example list
      openExample: undefined
    };
  }

  if (action.type === 'RNTesterExampleAction') {
    // Make sure we see the module before returning the new state
    const ExampleModule = RNTesterList.Modules[action.openExample];

    if (ExampleModule) {
      return {
        openExample: action.openExample
      };
    }
  }

  return state;
}

export default rnTesterNavigationReducer;
