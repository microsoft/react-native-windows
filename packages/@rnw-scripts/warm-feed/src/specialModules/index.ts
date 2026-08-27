/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Registry of pluggable special modules. Add a module here and it becomes
 * available to config (`closure.modules.<name>`) and `--closure-module <name>`.
 *
 * @format
 */

import type {SpecialModule} from './types';
import {createReactNativeLibraryModule} from './createReactNativeLibrary';

const modules = new Map<string, SpecialModule>();

export function registerSpecialModule(mod: SpecialModule): void {
  modules.set(mod.name, mod);
}

export function getSpecialModule(name: string): SpecialModule | undefined {
  return modules.get(name);
}

export function specialModuleNames(): string[] {
  return [...modules.keys()];
}

registerSpecialModule(createReactNativeLibraryModule);

export type {SpecialModule, SpecialModuleContext} from './types';
