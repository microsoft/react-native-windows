/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<dede865260673822ccfa2647eb1bd596>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\createAnimatedComponent.js
 */

import type { NativeColorValue } from "../StyleSheet/StyleSheetTypes";
import type AnimatedAddition from "./nodes/AnimatedAddition";
import type AnimatedDiffClamp from "./nodes/AnimatedDiffClamp";
import type AnimatedDivision from "./nodes/AnimatedDivision";
import type AnimatedInterpolation from "./nodes/AnimatedInterpolation";
import type AnimatedModulo from "./nodes/AnimatedModulo";
import type AnimatedMultiplication from "./nodes/AnimatedMultiplication";
import type AnimatedNode from "./nodes/AnimatedNode";
import type { AnimatedPropsAllowlist } from "./nodes/AnimatedProps";
import type AnimatedSubtraction from "./nodes/AnimatedSubtraction";
import type AnimatedValue from "./nodes/AnimatedValue";
import { type ViewProps } from "../Components/View/ViewPropTypes";
import * as React from "react";
type Nullable = void | null;
type Primitive = string | number | boolean | symbol | void;
type Builtin = (...$$REST$$: ReadonlyArray<never>) => unknown | Date | Error | RegExp;
type ComponentProps<T> = T extends React.ComponentType<infer Props> ? Props : never;
export type WithAnimatedValue<T> = T extends Builtin | Nullable ? T : T extends Primitive ? T | AnimatedNode | AnimatedAddition | AnimatedSubtraction | AnimatedDivision | AnimatedMultiplication | AnimatedModulo | AnimatedDiffClamp | AnimatedValue | AnimatedInterpolation<number | string> | AnimatedInterpolation<number> | AnimatedInterpolation<string> | AnimatedInterpolation<NativeColorValue> : T extends ReadonlyArray<infer P> ? ReadonlyArray<WithAnimatedValue<P>> : T extends {} ? { readonly [K in keyof T]: WithAnimatedValue<T[K]> } : T;
type NonAnimatedProps = "ref" | "innerViewRef" | "scrollViewRef" | "testID" | "disabled" | "accessibilityLabel";
type PassThroughProps = Readonly<{
  passthroughAnimatedPropExplicitValues?: ViewProps | null | undefined;
}>;
type LooseOmit<O extends {}, K extends string | number | symbol> = Pick<O, Exclude<keyof O, K>>;
export type AnimatedProps<Props extends {}> = LooseOmit<{ [K in keyof Props]: K extends NonAnimatedProps ? Props[K] : WithAnimatedValue<Props[K]> }, "ref"> & PassThroughProps;
export type AnimatedBaseProps<Props extends {}> = LooseOmit<{ [K in keyof Props]: K extends NonAnimatedProps ? Props[K] : WithAnimatedValue<Props[K]> }, "ref">;
export type AnimatedComponentType<Props extends {}, Instance = unknown> = (props: Omit<AnimatedProps<Props>, "ref"> & {
  ref?: React.Ref<Instance> | undefined;
}) => React.ReactNode;
declare function createAnimatedComponent<TInstance extends Exclude<React.ElementType, string>>(Component: TInstance): AnimatedComponentType<Readonly<ComponentProps<TInstance>>, React.ComponentRef<TInstance>>;
export default createAnimatedComponent;
export declare function unstable_createAnimatedComponentWithAllowlist<TProps extends {}, TInstance extends React.ComponentType<TProps>>(Component: TInstance, allowlist: null | undefined | AnimatedPropsAllowlist): AnimatedComponentType<TProps, React.ComponentRef<TInstance>>;
