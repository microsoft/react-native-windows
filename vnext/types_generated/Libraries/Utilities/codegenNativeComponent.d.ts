/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<279e94c7e5c829627e730d3981b9e9a9>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Utilities\codegenNativeComponent.js
 */

import type { HostComponent } from "../../src/private/types/HostComponent";
type NativeComponentOptions = Readonly<{
  interfaceOnly?: boolean | undefined;
  paperComponentName?: string | undefined;
  paperComponentNameDeprecated?: string | undefined;
  excludedPlatforms?: ReadonlyArray<"iOS" | "android"> | undefined;
  generateOptionalProperties?: boolean | undefined;
  generateOptionalObjectProperties?: boolean | undefined;
}>;
export type NativeComponentType<T extends {}> = HostComponent<T>;
declare function codegenNativeComponent<Props extends {}>(componentName: string, options?: NativeComponentOptions): HostComponent<Props>;
declare const $$codegenNativeComponent: typeof codegenNativeComponent;
declare type $$codegenNativeComponent = typeof $$codegenNativeComponent;
export default $$codegenNativeComponent;
