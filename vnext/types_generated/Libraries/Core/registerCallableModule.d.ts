/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<90389ef3721595fa1b55cc8171af7ef9>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Core\registerCallableModule.js
 */

type Module = {};
type RegisterCallableModule = (name: string, moduleOrFactory: Module | (($$PARAM_0$$: void) => Module)) => void;
declare const registerCallableModule: RegisterCallableModule;
declare const $$registerCallableModule: typeof registerCallableModule;
declare type $$registerCallableModule = typeof $$registerCallableModule;
export default $$registerCallableModule;
