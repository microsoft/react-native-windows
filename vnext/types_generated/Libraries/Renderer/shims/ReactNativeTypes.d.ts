/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<fba4baf248c359d506807392597452b0>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Renderer\shims\ReactNativeTypes.js
 */

import type { HostInstance as PublicInstance, MeasureOnSuccessCallback, PublicRootInstance, PublicTextInstance } from "react-native";
import * as React from "react";
export type AttributeType<T, V> = true | Readonly<{
  diff?: ((arg1: T, arg2: T) => boolean) | undefined;
  process?: ((arg1: V) => T) | undefined;
}>;
export type AnyAttributeType = AttributeType<any, any>;
export type AttributeConfiguration = Readonly<{
  [propName: string]: AnyAttributeType | void;
  style?: Readonly<{
    [propName: string]: AnyAttributeType;
  }> | undefined;
}>;
export type ViewConfig = Readonly<{
  Commands?: Readonly<{
    [commandName: string]: number;
  }> | undefined;
  Constants?: Readonly<{
    [name: string]: unknown;
  }> | undefined;
  Manager?: string | undefined;
  NativeProps?: Readonly<{
    [propName: string]: string;
  }> | undefined;
  baseModuleName?: string | undefined;
  bubblingEventTypes?: Readonly<{
    [eventName: string]: Readonly<{
      phasedRegistrationNames: Readonly<{
        captured: string;
        bubbled: string;
        skipBubbling?: boolean | undefined;
      }>;
    }>;
  }> | undefined;
  directEventTypes?: Readonly<{
    [eventName: string]: Readonly<{
      registrationName: string;
    }>;
  }> | undefined;
  supportsRawText?: boolean | undefined;
  uiViewClassName: string;
  validAttributes: AttributeConfiguration;
}>;
export type PartialViewConfig = Readonly<{
  bubblingEventTypes?: ViewConfig["bubblingEventTypes"] | undefined;
  directEventTypes?: ViewConfig["directEventTypes"] | undefined;
  supportsRawText?: boolean | undefined;
  uiViewClassName: string;
  validAttributes?: AttributeConfiguration | undefined;
}>;
type InspectorDataProps = Readonly<{
  [propName: string]: string;
}>;
type InspectorDataGetter = ($$PARAM_0$$: <TElementType extends React.ElementType>(componentOrHandle: React.ComponentRef<TElementType> | number) => null | undefined | number) => Readonly<{
  measure: (callback: MeasureOnSuccessCallback) => void;
  props: InspectorDataProps;
}>;
export type InspectorData = Readonly<{
  closestInstance?: unknown | undefined;
  hierarchy: Array<{
    name: string | undefined;
    getInspectorData: InspectorDataGetter;
  }>;
  selectedIndex: number | undefined;
  props: InspectorDataProps;
  componentStack: string;
}>;
export type TouchedViewDataAtPoint = Readonly<{
  pointerY: number;
  touchedViewTag?: number | undefined;
  frame: Readonly<{
    top: number;
    left: number;
    width: number;
    height: number;
  }>;
  closestPublicInstance?: PublicInstance | undefined;
} & InspectorData>;
export type RenderRootOptions = {
  onUncaughtError?: ((error: unknown, errorInfo: {
    readonly componentStack?: string | undefined;
  }) => void) | undefined;
  onCaughtError?: ((error: unknown, errorInfo: {
    readonly componentStack?: string | undefined;
    readonly errorBoundary?: React.Component<any, any> | undefined;
  }) => void) | undefined;
  onRecoverableError?: ((error: unknown, errorInfo: {
    readonly componentStack?: string | undefined;
  }) => void) | undefined;
  onDefaultTransitionIndicator?: (() => void | (() => void)) | undefined;
};
export declare type Node = symbol & {
  __Node__: string;
};
export declare type InternalInstanceHandle = symbol & {
  __InternalInstanceHandle__: string;
};
export type ReactFabricType = {
  findHostInstance_DEPRECATED<TElementType extends React.ElementType>(componentOrHandle: (React.ComponentRef<TElementType> | number) | undefined): PublicInstance | undefined;
  findNodeHandle<TElementType extends React.ElementType>(componentOrHandle: (React.ComponentRef<TElementType> | number) | undefined): number | undefined;
  dispatchCommand(handle: PublicInstance, command: string, args: Array<unknown>): void;
  isChildPublicInstance(parent: PublicInstance, child: PublicInstance): boolean;
  sendAccessibilityEvent(handle: PublicInstance, eventType: string): void;
  render(element: React.JSX.Element, containerTag: number, callback: (() => void) | undefined, concurrentRoot: boolean | undefined, options: RenderRootOptions | undefined): React.ComponentRef<React.ElementType> | undefined;
  unmountComponentAtNode(containerTag: number): void;
  getNodeFromInternalInstanceHandle(internalInstanceHandle: InternalInstanceHandle): Node | undefined;
  getPublicInstanceFromInternalInstanceHandle(internalInstanceHandle: InternalInstanceHandle): PublicInstance | PublicTextInstance | null;
  getPublicInstanceFromRootTag(rootTag: number): PublicRootInstance | null;
};
export type ReactFabricEventTouch = {
  identifier: number;
  locationX: number;
  locationY: number;
  pageX: number;
  pageY: number;
  screenX: number;
  screenY: number;
  target: number;
  timestamp: number;
  force: number;
};
export type ReactFabricEvent = {
  touches: Array<ReactFabricEventTouch>;
  changedTouches: Array<ReactFabricEventTouch>;
  targetTouches: Array<ReactFabricEventTouch>;
  target: number;
};
export type LayoutAnimationType = "spring" | "linear" | "easeInEaseOut" | "easeIn" | "easeOut" | "keyboard";
export type LayoutAnimationProperty = "opacity" | "scaleX" | "scaleY" | "scaleXY";
export type LayoutAnimationAnimationConfig = Readonly<{
  duration?: number | undefined;
  delay?: number | undefined;
  springDamping?: number | undefined;
  initialVelocity?: number | undefined;
  type?: LayoutAnimationType | undefined;
  property?: LayoutAnimationProperty | undefined;
}>;
export type LayoutAnimationConfig = Readonly<{
  duration: number;
  create?: LayoutAnimationAnimationConfig | undefined;
  update?: LayoutAnimationAnimationConfig | undefined;
  delete?: LayoutAnimationAnimationConfig | undefined;
}>;
