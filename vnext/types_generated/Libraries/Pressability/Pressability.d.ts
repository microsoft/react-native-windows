/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<541094f4959230aed9186e3e1c2ddff8>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Pressability\Pressability.js
 */

import type { BlurEvent, FocusEvent, GestureResponderEvent, MouseEvent } from "../Types/CoreEventTypes";
import { type RectOrSize } from "../StyleSheet/Rect";
import { type PointerEvent } from "../Types/CoreEventTypes";
export type PressabilityConfig = Readonly<{
  /**
   * Whether a press gesture can be interrupted by a parent gesture such as a
   * scroll event. Defaults to true.
   */
  cancelable?: boolean | undefined;
  /**
   * Whether to disable initialization of the press gesture.
   */
  disabled?: boolean | undefined;
  /**
   * Amount to extend the `VisualRect` by to create `HitRect`.
   */
  hitSlop?: RectOrSize | undefined;
  /**
   * Amount to extend the `HitRect` by to create `PressRect`.
   */
  pressRectOffset?: RectOrSize | undefined;
  /**
   * Whether to disable the systemm sound when `onPress` fires on Android.
   **/
  android_disableSound?: boolean | undefined;
  /**
   * Duration to wait after hover in before calling `onHoverIn`.
   */
  delayHoverIn?: number | undefined;
  /**
   * Duration to wait after hover out before calling `onHoverOut`.
   */
  delayHoverOut?: number | undefined;
  /**
   * Duration (in addition to `delayPressIn`) after which a press gesture is
   * considered a long press gesture. Defaults to 500 (milliseconds).
   */
  delayLongPress?: number | undefined;
  /**
   * Duration to wait after press down before calling `onPressIn`.
   */
  delayPressIn?: number | undefined;
  /**
   * Duration to wait after letting up before calling `onPressOut`.
   */
  delayPressOut?: number | undefined;
  /**
   * Minimum duration to wait between calling `onPressIn` and `onPressOut`.
   */
  minPressDuration?: number | undefined;
  /**
   * Called after the element loses focus.
   */
  onBlur?: ((event: BlurEvent) => unknown) | undefined;
  /**
   * Called after the element is focused.
   */
  onFocus?: ((event: FocusEvent) => unknown) | undefined;
  /**
   * Called when the hover is activated to provide visual feedback.
   */
  onHoverIn?: ((event: MouseEvent) => unknown) | undefined;
  /**
   * Called when the hover is deactivated to undo visual feedback.
   */
  onHoverOut?: ((event: MouseEvent) => unknown) | undefined;
  /**
   * Called when a long press gesture has been triggered.
   */
  onLongPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called when a press gesture has been triggered.
   */
  onPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called when the press is activated to provide visual feedback.
   */
  onPressIn?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called when the press location moves. (This should rarely be used.)
   */
  onPressMove?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called when the press is deactivated to undo visual feedback.
   */
  onPressOut?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Whether to prevent any other native components from becoming responder
   * while this pressable is responder.
   */
  blockNativeResponder?: boolean | undefined;
}>;
export type EventHandlers = Readonly<{
  onBlur: (event: BlurEvent) => void;
  onClick: (event: GestureResponderEvent) => void;
  onFocus: (event: FocusEvent) => void;
  onMouseEnter?: ((event: MouseEvent) => void) | undefined;
  onMouseLeave?: ((event: MouseEvent) => void) | undefined;
  onPointerEnter?: ((event: PointerEvent) => void) | undefined;
  onPointerLeave?: ((event: PointerEvent) => void) | undefined;
  onResponderGrant: (event: GestureResponderEvent) => void | boolean;
  onResponderMove: (event: GestureResponderEvent) => void;
  onResponderRelease: (event: GestureResponderEvent) => void;
  onResponderTerminate: (event: GestureResponderEvent) => void;
  onResponderTerminationRequest: () => boolean;
  onStartShouldSetResponder: () => boolean;
}>;
/**
 * Pressability implements press handling capabilities.
 *
 * =========================== Pressability Tutorial ===========================
 *
 * The `Pressability` class helps you create press interactions by analyzing the
 * geometry of elements and observing when another responder (e.g. ScrollView)
 * has stolen the touch lock. It offers hooks for your component to provide
 * interaction feedback to the user:
 *
 * - When a press has activated (e.g. highlight an element)
 * - When a press has deactivated (e.g. un-highlight an element)
 * - When a press should trigger an action, meaning it activated and deactivated
 *   while within the geometry of the element without the lock being stolen.
 *
 * A high quality interaction isn't as simple as you might think. There should
 * be a slight delay before activation. Moving your finger beyond an element's
 * bounds should trigger deactivation, but moving the same finger back within an
 * element's bounds should trigger reactivation.
 *
 * This should be consumed by functional components using `usePressability`. The
 * following steps are only relevant for using `Pressability` in classes:
 *
 * 1. Instantiate `Pressability` and store it on your component's state.
 *
 *    state = {
 *      pressability: new Pressability({
 *        // ...
 *      }),
 *    };
 *
 * 2. Choose the rendered component who should collect the press events. On that
 *    element, spread `pressability.getEventHandlers()` into its props.
 *
 *    return (
 *      <View {...this.state.pressability.getEventHandlers()} />
 *    );
 *
 * 3. Update `Pressability` when your component mounts, updates, and unmounts.
 *
 *    componentDidMount() {
 *      this.state.pressability.configure(...);
 *    }
 *
 *    componentDidUpdate() {
 *      this.state.pressability.configure(...);
 *    }
 *
 *    componentWillUnmount() {
 *      this.state.pressability.reset();
 *    }
 *
 * ==================== Pressability Implementation Details ====================
 *
 * `Pressability` only assumes that there exists a `HitRect` node. The `PressRect`
 * is an abstract box that is extended beyond the `HitRect`.
 *
 * # Geometry
 *
 *  ┌────────────────────────┐
 *  │  ┌──────────────────┐  │ - Presses start anywhere within `HitRect`, which
 *  │  │  ┌────────────┐  │  │   is expanded via the prop `hitSlop`.
 *  │  │  │ VisualRect │  │  │
 *  │  │  └────────────┘  │  │ - When pressed down for sufficient amount of time
 *  │  │    HitRect       │  │   before letting up, `VisualRect` activates for
 *  │  └──────────────────┘  │   as long as the press stays within `PressRect`.
 *  │       PressRect    o   │
 *  └────────────────────│───┘
 *          Out Region   └────── `PressRect`, which is expanded via the prop
 *                               `pressRectOffset`, allows presses to move
 *                               beyond `HitRect` while maintaining activation
 *                               and being eligible for a "press".
 *
 * # State Machine
 *
 * ┌───────────────┐ ◀──── RESPONDER_RELEASE
 * │ NOT_RESPONDER │
 * └───┬───────────┘ ◀──── RESPONDER_TERMINATED
 *     │
 *     │ RESPONDER_GRANT (HitRect)
 *     │
 *     ▼
 * ┌─────────────────────┐          ┌───────────────────┐              ┌───────────────────┐
 * │ RESPONDER_INACTIVE_ │  DELAY   │ RESPONDER_ACTIVE_ │  T + DELAY   │ RESPONDER_ACTIVE_ │
 * │ PRESS_IN            ├────────▶ │ PRESS_IN          ├────────────▶ │ LONG_PRESS_IN     │
 * └─┬───────────────────┘          └─┬─────────────────┘              └─┬─────────────────┘
 *   │           ▲                    │           ▲                      │           ▲
 *   │LEAVE_     │                    │LEAVE_     │                      │LEAVE_     │
 *   │PRESS_RECT │ENTER_              │PRESS_RECT │ENTER_                │PRESS_RECT │ENTER_
 *   │           │PRESS_RECT          │           │PRESS_RECT            │           │PRESS_RECT
 *   ▼           │                    ▼           │                      ▼           │
 * ┌─────────────┴───────┐          ┌─────────────┴─────┐              ┌─────────────┴─────┐
 * │ RESPONDER_INACTIVE_ │  DELAY   │ RESPONDER_ACTIVE_ │              │ RESPONDER_ACTIVE_ │
 * │ PRESS_OUT           ├────────▶ │ PRESS_OUT         │              │ LONG_PRESS_OUT    │
 * └─────────────────────┘          └───────────────────┘              └───────────────────┘
 *
 * T + DELAY => LONG_PRESS_DELAY + DELAY
 *
 * Not drawn are the side effects of each transition. The most important side
 * effect is the invocation of `onPress` and `onLongPress` that occur when a
 * responder is release while in the "press in" states.
 */
declare class Pressability {
  constructor(config: PressabilityConfig);
  configure(config: PressabilityConfig): void;
  /**
   * Resets any pending timers. This should be called on unmount.
   */
  reset(): void;
  /**
   * Returns a set of props to spread into the interactive element.
   */
  getEventHandlers(): EventHandlers;
  static setLongPressDeactivationDistance(distance: number): void;
}
export default Pressability;
