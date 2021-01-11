/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

export type TestComponent = string | SkippedTest;
export type SkippedTest = {skip: string};
export type TestBlock = {name: string; tests: TestComponent[]};

/**
 * Declare a test component to skip
 */
export function skip(component: string) {
  return {skip: component};
}

/**
 * Declare a grouped block of tests
 */
export function block(name: string, tests: TestComponent[]): TestBlock {
  return {name, tests};
}
