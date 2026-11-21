/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<0ee704187b87fc5060e3a47c1b1ba2bb>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\AppRegistryImpl.js
 */

import type { RootTag } from "../Types/RootTagTypes";
import type { AppConfig, AppParameters, ComponentProvider, ComponentProviderInstrumentationHook, Registry, RootViewStyleProvider, Runnable, Runnables, TaskProvider, WrapperComponentProvider } from "./AppRegistry.flow";
type TaskCanceller = () => void;
type TaskCancelProvider = () => TaskCanceller;
/**
 * Sets a provider for a wrapper component that will wrap the root component
 * of every registered app.
 */
export declare function setWrapperComponentProvider(provider: WrapperComponentProvider): void;
/**
 * Sets a provider for styles to be applied to the root view of every
 * registered app.
 */
export declare function setRootViewStyleProvider(provider: RootViewStyleProvider): void;
/**
 * Registers multiple apps with a single call by providing an array of app
 * configurations.
 */
export declare function registerConfig(config: Array<AppConfig>): void;
/**
 * Registers a root component for the given app key. Once registered, the
 * native system can run the app by calling `runApplication` with the same
 * key.
 *
 * See https://reactnative.dev/docs/appregistry#registercomponent
 */
export declare function registerComponent(appKey: string, componentProvider: ComponentProvider, section?: boolean): string;
/**
 * Registers a custom run function for the given app key.
 */
export declare function registerRunnable(appKey: string, run: Runnable): string;
/**
 * Registers a component as a navigable section.
 */
export declare function registerSection(appKey: string, component: ComponentProvider): void;
/**
 * Returns the app keys for all registered runnables.
 */
export declare function getAppKeys(): ReadonlyArray<string>;
/**
 * Returns the keys for all registered sections.
 */
export declare function getSectionKeys(): ReadonlyArray<string>;
/**
 * Returns a copy of the registered sections map.
 */
export declare function getSections(): Runnables;
/**
 * Returns the runnable registered for the given app key.
 */
export declare function getRunnable(appKey: string): null | undefined | Runnable;
/**
 * Returns the full registry of section keys and runnables.
 */
export declare function getRegistry(): Registry;
/**
 * Sets a hook that is called when a component provider is instrumented
 * during registration.
 */
export declare function setComponentProviderInstrumentationHook(hook: ComponentProviderInstrumentationHook): void;
/**
 * Loads the JavaScript bundle and runs the app registered under the given
 * key. This is called by the native system when it is ready to display the
 * app.
 *
 * See https://reactnative.dev/docs/appregistry#runapplication
 */
export declare function runApplication(appKey: string, appParameters: AppParameters, displayMode?: number): void;
/**
 * Updates the initial props for a surface that has already been rendered.
 */
export declare function setSurfaceProps(appKey: string, appParameters: AppParameters, displayMode?: number): void;
/**
 * Stops an application when a view should be destroyed. Should always be
 * called as a counterpart to `runApplication`.
 *
 * See https://reactnative.dev/docs/appregistry#unmountapplicationcomponentatroottag
 */
export declare function unmountApplicationComponentAtRootTag(rootTag: RootTag): void;
/**
 * Registers a headless task. A headless task is a bit of code that runs
 * without a UI, e.g. for background sync or push notifications.
 *
 * See https://reactnative.dev/docs/appregistry#registerheadlesstask
 */
export declare function registerHeadlessTask(taskKey: string, taskProvider: TaskProvider): void;
/**
 * Registers a cancellable headless task. A headless task is a bit of code
 * that runs without a UI. Unlike `registerHeadlessTask`, this variant
 * accepts a cancel provider that can be used to abort the task.
 *
 * See https://reactnative.dev/docs/appregistry#registercancellableheadlesstask
 */
export declare function registerCancellableHeadlessTask(taskKey: string, taskProvider: TaskProvider, taskCancelProvider: TaskCancelProvider): void;
/**
 * Starts a headless task. Called from native code when a registered headless
 * task should begin execution.
 *
 * See https://reactnative.dev/docs/appregistry#startheadlesstask
 */
export declare function startHeadlessTask(taskId: number, taskKey: string, data: unknown): void;
/**
 * Cancels a headless task. Called from native code when a previously started
 * headless task should be aborted.
 *
 * See https://reactnative.dev/docs/appregistry#cancelheadlesstask
 */
export declare function cancelHeadlessTask(taskId: number, taskKey: string): void;
