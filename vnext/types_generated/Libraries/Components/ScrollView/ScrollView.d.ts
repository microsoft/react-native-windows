/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<45b69d6839a875faf66507c7ba5985e0>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\ScrollView\ScrollView.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { EdgeInsetsProp } from "../../StyleSheet/EdgeInsetsPropType";
import type { PointProp } from "../../StyleSheet/PointPropType";
import type { ViewStyleProp } from "../../StyleSheet/StyleSheet";
import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { ScrollEvent } from "../../Types/CoreEventTypes";
import type { KeyboardEvent } from "../Keyboard/Keyboard";
import type { ViewProps } from "../View/ViewPropTypes";
import type { ScrollViewStickyHeaderProps } from "./ScrollViewStickyHeader";
import View from "../View/View";
import ScrollViewContext from "./ScrollViewContext";
import * as React from "react";
export interface ScrollViewScrollToOptions {
  x?: number | undefined;
  y?: number | undefined;
  animated?: boolean | undefined;
}
export interface ScrollViewImperativeMethods {
  /**
   * Returns a reference to the underlying scroll responder, which supports
   * operations like `scrollTo`. All ScrollView-like components should
   * implement this method so that they can be composed while providing access
   * to the underlying scroll responder's methods.
   */
  readonly getScrollResponder: () => ScrollResponderType;
  readonly getScrollableNode: () => number | undefined;
  readonly getInnerViewNode: () => number | undefined;
  readonly getInnerViewRef: () => InnerViewInstance | null;
  /**
   * Returns a reference to the underlying native scroll view, or null if the
   * native instance is not mounted.
   */
  readonly getNativeScrollRef: () => ScrollViewInstance | null;
  /**
   * Scrolls to a given x, y offset, either immediately or with a smooth animation.
   * Syntax:
   *
   * scrollTo(options: {x: number = 0; y: number = 0; animated: boolean = true})
   *
   * Note: The weird argument signature is due to the fact that, for historical reasons,
   * the function also accepts separate arguments as an alternative to the options object.
   * This is deprecated due to ambiguity (y before x), and SHOULD NOT BE USED.
   */
  readonly scrollTo: (options?: ScrollViewScrollToOptions | number, deprecatedX?: number, deprecatedAnimated?: boolean) => void;
  /**
   * A helper function that scrolls to the end of the scrollview;
   * If this is a vertical ScrollView, it scrolls to the bottom.
   * If this is a horizontal ScrollView scrolls to the right.
   *
   * The options object has an animated prop, that enables the scrolling animation or not.
   * The animated prop defaults to true
   */
  readonly scrollToEnd: (options?: ScrollViewScrollToOptions | undefined) => void;
  /**
   * Displays the scroll indicators momentarily.
   */
  readonly flashScrollIndicators: () => void;
  readonly scrollResponderZoomTo: (rect: {
    x: number;
    y: number;
    width: number;
    height: number;
    animated?: boolean | undefined;
  }, animated?: boolean) => void;
  readonly scrollResponderScrollNativeHandleToKeyboard: (nodeHandle: number | HostInstance, additionalOffset?: number, preventNegativeScrollOffset?: boolean) => void;
}
export type DecelerationRateType = "fast" | "normal" | number;
export type ScrollResponderType = ScrollViewImperativeMethods;
export interface ScrollViewInstance extends HostInstance, ScrollViewImperativeMethods {}
type InnerViewInstance = React.ComponentRef<typeof View>;
export type ScrollViewPropsIOS = Readonly<{
  /**
   * Controls whether iOS should automatically adjust the content inset
   * for scroll views that are placed behind a navigation bar or
   * tab bar/toolbar.
   *
   * @default `true`
   * @platform ios
   */
  automaticallyAdjustContentInsets?: boolean | undefined;
  /**
   * Controls whether the ScrollView should automatically adjust its `contentInset`
   * and `scrollViewInsets` when the Keyboard changes its size.
   *
   * @default `false`
   * @platform ios
   */
  automaticallyAdjustKeyboardInsets?: boolean | undefined;
  /**
   * Controls whether iOS should automatically adjust the scroll indicator
   * insets. Available on iOS 13 and later.
   *
   * @default `true`
   * @platform ios
   */
  automaticallyAdjustsScrollIndicatorInsets?: boolean | undefined;
  /**
   * The amount by which the scroll view content is inset from the edges
   * of the scroll view.
   *
   * @default `{top: 0, left: 0, bottom: 0, right: 0}`
   * @platform ios
   */
  contentInset?: EdgeInsetsProp | undefined;
  /**
   * When true, the scroll view bounces when it reaches the end of the
   * content if the content is larger than the scroll view along the axis of
   * the scroll direction. When `false`, it disables all bouncing even if
   * the `alwaysBounce*` props are `true`.
   *
   * @default `true`
   * @platform ios
   */
  bounces?: boolean | undefined;
  /**
   * By default, ScrollView has an active pan responder that hijacks panresponders
   * deeper in the render tree in order to prevent accidental touches while scrolling.
   * However, in certain occasions (such as when using snapToInterval) in a vertical scrollview
   * You may want to disable this behavior in order to prevent the ScrollView from blocking touches
   */
  disableScrollViewPanResponder?: boolean | undefined;
  /**
   * When true, gestures can drive zoom past min/max and the zoom will animate
   * to the min/max value at gesture end, otherwise the zoom will not exceed
   * the limits.
   * @platform ios
   */
  bouncesZoom?: boolean | undefined;
  /**
   * When true, the scroll view bounces horizontally when it reaches the end
   * even if the content is smaller than the scroll view itself.
   *
   * @default {@platform horizontal} `true`
   * @platform ios
   */
  alwaysBounceHorizontal?: boolean | undefined;
  /**
   * When true, the scroll view bounces vertically when it reaches the end
   * even if the content is smaller than the scroll view itself.
   *
   * @default {@platform horizontal} `false`
   * @platform ios
   */
  alwaysBounceVertical?: boolean | undefined;
  /**
   * When true, the scroll view automatically centers the content when the
   * content is smaller than the scroll view bounds; when the content is
   * larger than the scroll view, this property has no effect.
   *
   * @default `false`
   * @platform ios
   */
  centerContent?: boolean | undefined;
  /**
   * The style of the scroll indicators.
   *
   *   - `'default'` (the default), same as `black`.
   *   - `'black'`, scroll indicator is black. This style is good against a light background.
   *   - `'white'`, scroll indicator is white. This style is good against a dark background.
   *
   * @platform ios
   */
  indicatorStyle?: ("default" | "black" | "white") | undefined;
  /**
   * When true, the ScrollView will try to lock to only vertical or horizontal
   * scrolling while dragging.
   *
   * @default `false`
   * @platform ios
   */
  directionalLockEnabled?: boolean | undefined;
  /**
   * When false, once tracking starts, the scroll view will not try to drag
   * if the touch moves.
   *
   * @default `true`
   * @platform ios
   */
  canCancelContentTouches?: boolean | undefined;
  /**
   * The maximum allowed zoom scale.
   *
   * @default `1.0`
   * @platform ios
   */
  maximumZoomScale?: number | undefined;
  /**
   * The minimum allowed zoom scale.
   *
   * @default `1.0`
   * @platform ios
   */
  minimumZoomScale?: number | undefined;
  /**
   * When true, ScrollView allows use of pinch gestures to zoom in and out.
   *
   * @default `true`
   * @platform ios
   */
  pinchGestureEnabled?: boolean | undefined;
  /**
   * The amount by which the scroll view indicators are inset from the edges
   * of the scroll view. This should normally be set to the same value as
   * the `contentInset`.
   *
   * @default `{top: 0, left: 0, bottom: 0, right: 0}`
   * @platform ios
   */
  scrollIndicatorInsets?: EdgeInsetsProp | undefined;
  /**
   * When true, the scroll view can be programmatically scrolled beyond its
   * content size.
   *
   * @default `false`
   * @platform ios
   */
  scrollToOverflowEnabled?: boolean | undefined;
  /**
   * When true, the scroll view scrolls to top when the status bar is tapped.
   *
   * @default `true`
   * @platform ios
   */
  scrollsToTop?: boolean | undefined;
  /**
   * Fires when the scroll view scrolls to top after the status bar has been tapped
   * @platform ios
   */
  onScrollToTop?: ((event: ScrollEvent) => void) | undefined;
  /**
   * When true, shows a horizontal scroll indicator.
   *
   * @default `true`
   */
  showsHorizontalScrollIndicator?: boolean | undefined;
  /**
   * The current scale of the scroll view content.
   *
   * @default `1.0`
   * @platform ios
   */
  zoomScale?: number | undefined;
  /**
   * This property specifies how the safe area insets are used to modify the
   * content area of the scroll view.
   *
   * @default `'never'`
   * @platform ios
   */
  contentInsetAdjustmentBehavior?: ("automatic" | "scrollableAxes" | "never" | "always") | undefined;
}>;
export type ScrollViewPropsAndroid = Readonly<{
  /**
   * Enables nested scrolling for Android API level 21+. Nested scrolling is
   * supported by default on iOS.
   *
   * @default `false`
   * @platform android
   */
  nestedScrollEnabled?: boolean | undefined;
  /**
   * Sometimes a scrollview takes up more space than its content fills. When this is
   * the case, this prop will fill the rest of the scrollview with a color to avoid setting
   * a background and creating unnecessary overdraw. This is an advanced optimization
   * that is not needed in the general case.
   * @platform android
   */
  endFillColor?: ColorValue | undefined;
  /**
   * Tag used to log scroll performance on this scroll view. Will force
   * momentum events to be turned on (see sendMomentumEvents). This doesn't do
   * anything out of the box and you need to implement a custom native
   * FpsListener for it to be useful.
   * @platform android
   */
  scrollPerfTag?: string | undefined;
  /**
   * Used to override default value of overScroll mode.
   *
   * Possible values:
   *
   *  - `'auto'` - Default value, allow a user to over-scroll
   *    this view only if the content is large enough to meaningfully scroll.
   *  - `'always'` - Always allow a user to over-scroll this view.
   *  - `'never'` - Never allow a user to over-scroll this view.
   *
   * @platform android
   */
  overScrollMode?: ("auto" | "always" | "never") | undefined;
  /**
   * Causes the scrollbars not to turn transparent when they are not in use.
   *
   * @default `false`
   * @platform android
   */
  persistentScrollbar?: boolean | undefined;
  /**
   * Controls the fading effect at the edges of the scroll content.
   *
   * A value greater than 0 will apply the fading effect, indicating more content is available
   * to scroll.
   *
   * You can specify a single number to apply the same fading length to both edges.
   * Alternatively, use an object with `start` and `end` properties to set different
   * fading lengths for the start and end of the scroll content.
   *
   * The default value is 0.
   *
   * @platform android
   */
  fadingEdgeLength?: (number | undefined) | {
    start: number;
    end: number;
  } | undefined;
  /**
   * When false, the ScrollView will not automatically scroll to a focused child when
   * the child requests focus. This can be useful when you want to control the scroll
   * position programmatically.
   *
   * @default `true`
   * @platform android
   */
  scrollsChildToFocus?: boolean | undefined;
}>;
type StickyHeaderComponentType = (props: Omit<ScrollViewStickyHeaderProps, "ref"> & {
  ref?: React.Ref<Readonly<{
    setNextHeaderY: ($$PARAM_0$$: number) => void;
  }>> | undefined;
}) => React.ReactNode;
type ScrollViewBaseProps = Readonly<{
  /**
   * These styles will be applied to the scroll view content container which
   * wraps all of the child views.
   *
   * Example:
   *
   * ```tsx
   * return (
   *   <ScrollView contentContainerStyle={styles.contentContainer}>
   *   </ScrollView>
   * );
   * ...
   * const styles = StyleSheet.create({
   *   contentContainer: {
   *     paddingVertical: 20
   *   }
   * });
   * ```
   */
  contentContainerStyle?: ViewStyleProp | undefined;
  /**
   * Used to manually set the starting scroll offset.
   *
   * @default `{x: 0, y: 0}`
   */
  contentOffset?: PointProp | undefined;
  /**
   * When true, the scroll view stops on the next index (in relation to scroll
   * position at release) regardless of how fast the gesture is. This can be
   * used for pagination when the page is less than the width of the
   * horizontal ScrollView or the height of the vertical ScrollView.
   *
   * @default `false`
   */
  disableIntervalMomentum?: boolean | undefined;
  /**
   * A floating-point number that determines how quickly the scroll view
   * decelerates after the user lifts their finger. You may also use string
   * shortcuts `"normal"` and `"fast"` which match the underlying iOS settings
   * for `UIScrollViewDecelerationRateNormal` and
   * `UIScrollViewDecelerationRateFast` respectively.
   *
   *   - `'normal'`: 0.998 on iOS, 0.985 on Android
   *   - `'fast'`: 0.99 on iOS, 0.9 on Android
   *
   * @default `'normal'`
   */
  decelerationRate?: DecelerationRateType | undefined;
  /**
   * *Experimental, iOS Only*. The API is experimental and will change in future releases.
   *
   * Controls how much distance is traveled after user stops scrolling.
   * A value greater than 1 will increase the distance traveled.
   * A value less than 1 will decrease the distance traveled.
   *
   * @deprecated
   *
   * @default `1`
   */
  experimental_endDraggingSensitivityMultiplier?: number | undefined;
  /**
   * When true, the scroll view's children are arranged horizontally in a row
   * instead of vertically in a column.
   *
   * @default `false`
   */
  horizontal?: boolean | undefined;
  /**
   * If sticky headers should stick at the bottom instead of the top of the
   * ScrollView. This is usually used with inverted ScrollViews.
   */
  invertStickyHeaders?: boolean | undefined;
  /**
   * Determines whether the keyboard gets dismissed in response to a drag.
   *
   * *Cross platform*
   *
   *   - `'none'`, drags do not dismiss the keyboard.
   *   - `'on-drag'`, the keyboard is dismissed when a drag begins.
   *
   * *iOS Only*
   *
   *   - `'interactive'`, the keyboard is dismissed interactively with the drag and moves in
   *     synchrony with the touch; dragging upwards cancels the dismissal.
   *     On Android this is not supported and it will have the same behavior as `'none'`.
   *
   * @default `'none'`
   */
  keyboardDismissMode?: ("none" | "on-drag" | "interactive") | undefined;
  /**
   * Determines when the keyboard should stay visible after a tap.
   *
   *   - `'never'`, tapping outside of the focused text input when the keyboard
   *     is up dismisses the keyboard. When this happens, children will not receive the tap.
   *   - `'always'`, the keyboard will not dismiss automatically, and the scroll view will not
   *     catch taps, but children of the scroll view can catch taps.
   *   - `'handled'`, the keyboard will not dismiss automatically when the tap was handled by
   *     children, (or captured by an ancestor).
   *
   * @default `'never'`
   */
  keyboardShouldPersistTaps?: ("always" | "never" | "handled") | undefined;
  /**
   * When set, the scroll view will adjust the scroll position so that the first child that is
   * partially or fully visible and at or beyond `minIndexForVisible` will not change position.
   * This is useful for lists that are loading content in both directions, e.g. a chat thread,
   * where new messages coming in might otherwise cause the scroll position to jump. A value of 0
   * is common, but other values such as 1 can be used to skip loading spinners or other content
   * that should not maintain position.
   *
   * The optional `autoscrollToTopThreshold` can be used to make the content automatically scroll
   * to the top after making the adjustment if the user was within the threshold of the top before
   * the adjustment was made. This is also useful for chat-like applications where you want to see
   * new messages scroll into place, but not if the user has scrolled up a ways and it would be
   * disruptive to scroll a bunch.
   *
   * Caveat 1: Reordering elements in the scrollview with this enabled will probably cause
   * jumpiness and jank. It can be fixed, but there are currently no plans to do so. For now,
   * don't re-order the content of any ScrollViews or Lists that use this feature.
   *
   * Caveat 2: This simply uses `contentOffset` and `frame.origin` in native code to compute
   * visibility. Occlusion, transforms, and other complexity won't be taken into account as to
   * whether content is "visible" or not.
   *
   */
  maintainVisibleContentPosition?: Readonly<{
    minIndexForVisible: number;
    autoscrollToTopThreshold?: number | undefined;
  }> | undefined;
  /**
   * Called when the momentum scroll starts (scroll which occurs as the ScrollView glides to a stop).
   */
  onMomentumScrollBegin?: ((event: ScrollEvent) => void) | undefined;
  /**
   * Called when the momentum scroll ends (scroll which occurs as the ScrollView glides to a stop).
   */
  onMomentumScrollEnd?: ((event: ScrollEvent) => void) | undefined;
  /**
   * Fires at most once per frame during scrolling.
   */
  onScroll?: ((event: ScrollEvent) => void) | undefined;
  /**
   * Called when the user begins to drag the scroll view.
   */
  onScrollBeginDrag?: ((event: ScrollEvent) => void) | undefined;
  /**
   * Called when the user stops dragging the scroll view and it either stops
   * or begins to glide.
   */
  onScrollEndDrag?: ((event: ScrollEvent) => void) | undefined;
  /**
   * Called when scrollable content view of the ScrollView changes.
   *
   * Handler function is passed the content width and content height as parameters:
   * `(contentWidth, contentHeight)`
   *
   * It's implemented using onLayout handler attached to the content container
   * which this ScrollView renders.
   */
  onContentSizeChange?: ((contentWidth: number, contentHeight: number) => void) | undefined;
  onKeyboardDidShow?: ((event: KeyboardEvent) => void) | undefined;
  onKeyboardDidHide?: ((event: KeyboardEvent) => void) | undefined;
  onKeyboardWillShow?: ((event: KeyboardEvent) => void) | undefined;
  onKeyboardWillHide?: ((event: KeyboardEvent) => void) | undefined;
  /**
   * When true, the scroll view stops on multiples of the scroll view's size
   * when scrolling. This can be used for horizontal pagination.
   *
   * @default `false`
   */
  pagingEnabled?: boolean | undefined;
  /**
   * When false, the view cannot be scrolled via touch interaction.
   * Note that the view can always be scrolled by calling `scrollTo`.
   *
   * @default `true`
   */
  scrollEnabled?: boolean | undefined;
  /**
   * Limits how often scroll events will be fired while scrolling, specified as
   * a time interval in ms. This may be useful when expensive work is performed
   * in response to scrolling. Values <= `16` will disable throttling,
   * regardless of the refresh rate of the device.
   *
   * @default `0`
   */
  scrollEventThrottle?: number | undefined;
  /**
   * When true, shows a vertical scroll indicator.
   *
   * @default `true`
   */
  showsVerticalScrollIndicator?: boolean | undefined;
  /**
   * When true, the sticky header is hidden when scrolling down, and docks
   * at the top when scrolling up.
   *
   * @default `false`
   */
  stickyHeaderHiddenOnScroll?: boolean | undefined;
  /**
   * An array of child indices determining which children get docked to the
   * top of the screen when scrolling. For example, passing
   * `stickyHeaderIndices={[0]}` will cause the first child to be fixed to the
   * top of the scroll view. This property is not supported in conjunction
   * with `horizontal={true}`.
   */
  stickyHeaderIndices?: ReadonlyArray<number> | undefined;
  /**
   * A React Component that will be used to render sticky headers.
   * To be used together with `stickyHeaderIndices` or with `SectionList`, defaults to `ScrollViewStickyHeader`.
   * You may need to set this if your sticky header uses custom transforms (eg. translation),
   * for example when you want your list to have an animated hidable header.
   */
  StickyHeaderComponent?: StickyHeaderComponentType | undefined;
  /**
   * When `snapToInterval` is set, `snapToAlignment` defines the relationship
   * of the snapping to the scroll view.
   *
   *   - `'start'`, aligns the snap at the left (horizontal) or top (vertical).
   *   - `'center'`, aligns the snap in the center.
   *   - `'end'`, aligns the snap at the right (horizontal) or bottom (vertical).
   *
   * @default `'start'`
   */
  snapToAlignment?: ("start" | "center" | "end") | undefined;
  /**
   * When set, causes the scroll view to stop at multiples of the value of
   * `snapToInterval`. This can be used for paginating through children
   * that have lengths smaller than the scroll view. Typically used in
   * combination with `snapToAlignment` and `decelerationRate="fast"`.
   *
   * Overrides less configurable `pagingEnabled` prop.
   */
  snapToInterval?: number | undefined;
  /**
   * When set, causes the scroll view to stop at the defined offsets.
   * This can be used for paginating through variously sized children
   * that have lengths smaller than the scroll view. Typically used in
   * combination with `decelerationRate="fast"`.
   *
   * Overrides less configurable `pagingEnabled` and `snapToInterval` props.
   */
  snapToOffsets?: ReadonlyArray<number> | undefined;
  /**
   * Use in conjunction with `snapToOffsets`. By default, the beginning
   * of the list counts as a snap offset. Set `snapToStart` to `false` to disable
   * this behavior and allow the list to scroll freely between its start and
   * the first `snapToOffsets` offset.
   *
   * @default `true`
   */
  snapToStart?: boolean | undefined;
  /**
   * Use in conjunction with `snapToOffsets`. By default, the end
   * of the list counts as a snap offset. Set `snapToEnd` to `false` to disable
   * this behavior and allow the list to scroll freely between its end and
   * the last `snapToOffsets` offset.
   *
   * @default `true`
   */
  snapToEnd?: boolean | undefined;
  /**
   * Experimental: When true, offscreen child views (whose `overflow` value is
   * `hidden`) are removed from their native backing superview when offscreen.
   * This can improve scrolling performance on long lists.
   *
   * @default `true`
   */
  removeClippedSubviews?: boolean | undefined;
  /**
   * A RefreshControl component, used to provide pull-to-refresh
   * functionality for the ScrollView. Only works for vertical ScrollViews
   * (`horizontal` prop must be `false`).
   *
   * See [RefreshControl](docs/refreshcontrol.html).
   */
  refreshControl?: React.JSX.Element | undefined;
  children?: React.ReactNode | undefined;
  /**
   * A ref to the inner View element of the ScrollView. This should be used
   * instead of calling `getInnerViewRef`.
   */
  innerViewRef?: React.Ref<InnerViewInstance> | undefined;
  /**
   * A ref to the Native ScrollView component. This ref can be used to call
   * all of ScrollView's public methods, in addition to native methods like
   * measure, measureLayout, etc.
   */
  scrollViewRef?: React.Ref<ScrollViewInstance> | undefined;
}>;
export interface ScrollViewProps extends Readonly<Omit<Omit<ViewProps, "experimental_accessibilityOrder">, keyof ScrollViewPropsIOS | keyof ScrollViewPropsAndroid | keyof ScrollViewBaseProps | keyof {}> & Omit<ScrollViewPropsIOS, keyof ScrollViewPropsAndroid | keyof ScrollViewBaseProps | keyof {}> & Omit<ScrollViewPropsAndroid, keyof ScrollViewBaseProps | keyof {}> & Omit<ScrollViewBaseProps, keyof {}>> {}
export type ScrollViewComponentStatics = Readonly<{
  Context: typeof ScrollViewContext;
}>;
declare const ScrollViewWrapper: (props: Omit<ScrollViewProps, "ref"> & {
  ref?: React.Ref<ScrollViewInstance> | undefined;
}) => React.ReactNode;
/**
 * Component that wraps platform ScrollView while providing
 * integration with touch locking "responder" system.
 *
 * Keep in mind that ScrollViews must have a bounded height in order to work,
 * since they contain unbounded-height children into a bounded container (via
 * a scroll interaction). In order to bound the height of a ScrollView, either
 * set the height of the view directly (discouraged) or make sure all parent
 * views have bounded height. Forgetting to transfer `{flex: 1}` down the
 * view stack can lead to errors here, which the element inspector makes
 * easy to debug.
 *
 * Doesn't yet support other contained responders from blocking this scroll
 * view from becoming the responder.
 *
 *
 * `<ScrollView>` vs [`<FlatList>`](https://reactnative.dev/docs/flatlist) - which one to use?
 *
 * `ScrollView` simply renders all its react child components at once. That
 * makes it very easy to understand and use.
 *
 * On the other hand, this has a performance downside. Imagine you have a very
 * long list of items you want to display, maybe several screens worth of
 * content. Creating JS components and native views for everything all at once,
 * much of which may not even be shown, will contribute to slow rendering and
 * increased memory usage.
 *
 * This is where `FlatList` comes into play. `FlatList` renders items lazily,
 * just when they are about to appear, and removes items that scroll way off
 * screen to save memory and processing time.
 *
 * `FlatList` is also handy if you want to render separators between your items,
 * multiple columns, infinite scroll loading, or any number of other features it
 * supports out of the box.
 *
 * @see https://reactnative.dev/docs/scrollview
 */
declare const $$ScrollView: typeof ScrollViewWrapper & ScrollViewComponentStatics;
declare type $$ScrollView = typeof $$ScrollView;
export default $$ScrollView;
