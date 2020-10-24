/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

import type {RNTesterExample} from '../types/RNTesterTypes';

type Context = $FlowFixMe;

export const initializeAsyncStore = (_context: Context) => {
  // Stubbed on Win32 due to lack of linking + AsyncStorage in Rex
};

export const addApi = (
  apiName: string,
  api: RNTesterExample,
  context: $FlowFixMe,
) => {
  const stateApi = Object.assign({}, context.state.Api);
  stateApi[apiName] = api;
  context.setState({
    Api: stateApi,
  });
};

export const addComponent = (
  componentName: string,
  component: RNTesterExample,
  context: Context,
) => {
  const stateComponent = Object.assign({}, context.state.Components);
  stateComponent[componentName] = component;
  context.setState({
    Components: stateComponent,
  });
  // Syncing the bookmarks over async storage
  // AsyncStorage.setItem('Components', JSON.stringify(stateComponent)); [Win32]
};

export const removeApi = (apiName: string, context: Context) => {
  const stateApi = Object.assign({}, context.state.Api);
  delete stateApi[apiName];
  context.setState({
    Api: stateApi,
  });
  // AsyncStorage.setItem('Api', JSON.stringify(stateApi)); [Win32]
};

export const removeComponent = (componentName: string, context: Context) => {
  const stateComponent = Object.assign({}, context.state.Components);
  delete stateComponent[componentName];
  context.setState({
    Components: stateComponent,
  });
  // AsyncStorage.setItem('Components', JSON.stringify(stateComponent)); [Win32]
};

export const checkBookmarks = (
  title: string,
  key: string,
  context: Context,
): boolean => {
  if (key === 'APIS' || key === 'RECENT_APIS') {
    return context.state.Api[title] === undefined;
  }
  return context.state.Components[title] === undefined;
};

export const updateRecentlyViewedList = (
  item: RNTesterExample,
  key: string,
  context: Context,
) => {
  const openedItem = item;
  if (key === 'COMPONENTS' || key === 'RECENT_COMPONENTS') {
    let componentsCopy = [...context.state.recentComponents];
    const ind = componentsCopy.findIndex(
      component => component.key === openedItem.key,
    );
    if (ind !== -1) {
      componentsCopy.splice(ind, 1);
    }
    if (context.state.recentComponents.length >= 5) {
      componentsCopy.pop();
    }
    componentsCopy.unshift(openedItem);
    context.setState({
      recentComponents: componentsCopy,
    });
    // Syncing the recently viewed components over async storage
    // AsyncStorage.setItem('RecentComponents', JSON.stringify(componentsCopy)); [Win32]
  } else {
    let apisCopy = [...context.state.recentApis];
    const ind = apisCopy.findIndex(api => api.key === openedItem.key);
    if (ind !== -1) {
      apisCopy.splice(ind, 1);
    }
    if (context.state.recentApis.length >= 5) {
      apisCopy.pop();
    }
    apisCopy.unshift(openedItem);
    context.setState({
      recentApis: apisCopy,
    });
    // Syncing the recently viewed apis over async storage
    // AsyncStorage.setItem('RecentApi', JSON.stringify(apisCopy)); [Win32]
  }
};
