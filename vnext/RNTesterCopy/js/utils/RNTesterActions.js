/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

'use strict';

export type RNTesterBackAction = {
  type: 'RNTesterBackAction',
};

export type RNTesterListAction = {
  type: 'RNTesterListAction',
};

export type RNTesterExampleAction = {
  type: 'RNTesterExampleAction',
  openExample: string,
};

export type RNTesterAction =
  | RNTesterBackAction
  | RNTesterListAction
  | RNTesterExampleAction;

function Back(): RNTesterBackAction {
  return {
    type: 'RNTesterBackAction',
  };
}

function ExampleList(): RNTesterListAction {
  return {
    type: 'RNTesterListAction',
  };
}

function ExampleAction(openExample: string): RNTesterExampleAction {
  return {
    type: 'RNTesterExampleAction',
    openExample,
  };
}

const RNTesterActions = {
  Back,
  ExampleList,
  ExampleAction,
};

module.exports = RNTesterActions;
