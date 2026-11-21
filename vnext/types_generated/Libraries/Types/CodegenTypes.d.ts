/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<2277bd0e1a59f5ad2aaaffc218599ca7>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Types\CodegenTypes.js
 */

import type { EventSubscription } from "../vendor/emitter/EventEmitter";
import type { NativeSyntheticEvent } from "./CoreEventTypes";
export type BubblingEventHandler<T, PaperName extends string | never = never> = (event: NativeSyntheticEvent<T>) => void | Promise<void>;
export type DirectEventHandler<T, PaperName extends string | never = never> = (event: NativeSyntheticEvent<T>) => void | Promise<void>;
export type Double = number;
export type Float = number;
export type Int32 = number;
export type UnsafeObject = Object;
export type UnsafeMixed = unknown;
type DefaultTypes = number | boolean | string | ReadonlyArray<string>;
export type WithDefault<Type extends DefaultTypes, Value extends (null | undefined | Type) | string> = null | undefined | Type;
export type EventEmitter<T> = (handler: ($$PARAM_0$$: T) => void | Promise<void>) => EventSubscription;
