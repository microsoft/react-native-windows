// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
export interface IRNTesterBackAction {
  // tslint:disable-next-line no-reserved-keywords
  type: 'RNTesterBackAction';
}

export interface IRNTesterListAction {
  // tslint:disable-next-line no-reserved-keywords
  type: 'RNTesterListAction';
}

export interface IRNTesterExampleAction {
  // tslint:disable-next-line no-reserved-keywords
  type: 'RNTesterExampleAction';
  openExample: string;
}

export interface IRNTesterInitialAction {
  // tslint:disable-next-line no-reserved-keywords
  type: 'InitialAction';
}

export type RNTesterAction = IRNTesterBackAction | IRNTesterListAction | IRNTesterExampleAction | IRNTesterInitialAction;
