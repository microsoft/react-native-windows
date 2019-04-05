// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using System;
using System.Linq;
#if WINDOWS_UWP
using ReactNative.Accessibility;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
#endif

namespace ReactNative.Views.ControlView
{
    /// <summary>
    /// View manager for React control instances.
    /// </summary>
    public class ReactControlManager : ViewParentManager<ReactControl, ReactControlShadowNode>
    {
        private const int FocusCommand = 1;
        private const int BlurCommand = 2;

        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string  Name
        {
            get
            {
                return "WindowsControl";
            }
        }

        /// <summary>
        /// The commands.
        /// </summary>
        public override JObject ViewCommandsMap
        {
            get
            {
                return new JObject
                {
                    { "focus", FocusCommand },
                    { "blur", BlurCommand },
                };
            }
        }

        /// <summary>
        /// Exported view constants.
        /// </summary>
        public override JObject ViewConstants
        {
            get
            {
                return new JObject
                {
                    { "Keys", KeyHelpers.GetKeyConstants() },
                };
            }
        }

        /// <summary>
        /// Sets whether the view is a tab stop.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="isTabStop">
        /// <code>true</code> if the view is a tab stop, otherwise <code>false</code> (control can't get keyboard focus or accept keyboard input in this case).
        /// </param>
        [ReactProp("isTabStop")]
        public void SetIsTabStop(ReactControl view, bool isTabStop)
        {
#if !WINDOWS_UWP
            // Keep WPF consistent with UWP (so a control is either fully focusable (including by tabbing), or not at all.
            // This can be made more granular (for the WPF case) if a new prop is exposed.
            view.Focusable = isTabStop;
#endif
            view.IsTabStop = isTabStop;
        }

        /// <summary>
        /// Sets the tab index for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="tabIndex">The tab index.</param>
        [ReactProp("tabIndex")]
        public void SetTabIndex(ReactControl view, int tabIndex)
        {
            view.TabIndex = tabIndex;
        }

#if WINDOWS_UWP
        /// <summary>
        /// Sets the tab navigation for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="tabNavigation">The tab navigation.</param>
        [ReactProp("tabNavigation")]
        public void SetTabNavigation(ReactControl view, string tabNavigation)
        {
            view.TabNavigation = EnumHelpers.ParseNullable<KeyboardNavigationMode>(tabNavigation) ?? KeyboardNavigationMode.Local;
        }

        /// <summary>
        /// Sets the visuals to be used for focus rectangle.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="disableSystemFocusVisuals">
        /// <code>true</code> if the view should not show the system provided focus rect, otherwise <code>false</code>.
        /// </param>
        [ReactProp("disableSystemFocusVisuals")]
        public void SetDisableSystemFocusVisuals(ReactControl view, bool disableSystemFocusVisuals)
        {
            view.UseSystemFocusVisuals = !disableSystemFocusVisuals;
        }
#endif

        /// <summary>
        /// Sets the keys to handle during the <see cref="UIElement.KeyDownEvent"/>. 
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="keys">The keys.</param>
        [ReactProp("handledKeyDownKeys")]
        public void SetHandledKeyDownKeys(ReactControl view, int[] keys)
        {
            view.HandledKeyDownKeys = keys;
        }

        /// <summary>
        /// Sets the keys to handle during the <see cref="UIElement.KeyUpEvent"/>. 
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="keys">The keys.</param>
        [ReactProp("handledKeyUpKeys")]
        public void SetHandledKeyUpKeys(ReactControl view, int[] keys)
        {
            view.HandledKeyUpKeys = keys;
        }

#if WINDOWS_UWP
        /// <summary>
        /// Set accessibility traits for the control.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="accessibilityTraitsValue">Can be <see cref="JArray"/> of objects or a single object. 
        ///     String representation of the object(s) is parsed as <see cref="AccessibilityTrait"/>.</param>
        [ReactProp(ViewProps.AccessibilityTraits)]
        public void SetAccessibilityTraits(ReactControl view, object accessibilityTraitsValue)
        {
            AccessibilityHelper.SetAccessibilityTraits(view, accessibilityTraitsValue);
        }

        /// <summary>
        /// Sets <see cref="ImportantForAccessibility"/> for the ReactControl.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="importantForAccessibilityValue">The string to be parsed as <see cref="ImportantForAccessibility"/>.</param>
        [ReactProp(ViewProps.ImportantForAccessibility)]
        public void SetImportantForAccessibility(ReactControl view, string importantForAccessibilityValue)
        {
            var importantForAccessibility = EnumHelpers.ParseNullable<ImportantForAccessibility>(importantForAccessibilityValue) ?? ImportantForAccessibility.Auto;
            AccessibilityHelper.SetImportantForAccessibility(view, importantForAccessibility);
        }
#endif

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(ReactControl parent, DependencyObject child, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), "WindowsControl only supports one child.");
            }

            if (parent.Children.Count > 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), "WindowsControl already has a child element.");
            }

            var uiElementChild = child.As<UIElement>();
            parent.Children.Insert(index, uiElementChild);
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public override DependencyObject GetChildAt(ReactControl parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), "WindowsControl only supports one child.");
            }

            return parent.Children[index];
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(ReactControl parent)
        {
            return parent.Children.Count;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(ReactControl parent)
        {
            parent.Children.Clear();
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(ReactControl parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), "WindowsControl only supports one child.");
            }

            parent.Children.RemoveAt(index);
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override ReactControl CreateViewInstance(ThemedReactContext reactContext)
        {
            return new ReactControl();
        }

        /// <summary>
        /// Creates a shadow node instance for the view manager.
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public sealed override ReactControlShadowNode CreateShadowNodeInstance()
        {
            return new ReactControlShadowNode();
        }

        /// <summary>
        /// Receive a command from JavaScript.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public override void ReceiveCommand(ReactControl view, int commandId, JArray args)
        {
            if (commandId == FocusCommand)
            {
#if WINDOWS_UWP
                view.Focus(FocusState.Programmatic);
#else
                view.Focus();
#endif
            }
            else if (commandId == BlurCommand)
            {
#if WINDOWS_UWP
                var frame = Window.Current?.Content as Frame;
                frame?.Focus(FocusState.Programmatic);
#else
                Keyboard.ClearFocus();
#endif
            }
        }

        /// <summary>
        /// Subclasses can override this method to install custom event 
        /// emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        /// <remarks>
        /// Consider overriding this method if your view needs to emit events
        /// besides basic touch events to JavaScript (e.g., scroll events).
        /// </remarks>
        protected override void AddEventEmitters(ThemedReactContext reactContext, ReactControl view)
        {
            base.AddEventEmitters(reactContext, view);
            view.GotFocus += OnGotFocus;
            view.LostFocus += OnLostFocus;
            view.KeyUp += OnKeyUp;
            view.KeyDown += OnKeyDown;
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="IViewManager"/> subclass.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        /// <remarks>
        /// Be sure to call this base class method to register for pointer 
        /// entered and pointer exited events.
        /// </remarks>
        public override void OnDropViewInstance(ThemedReactContext reactContext, ReactControl view)
        {
            view.GotFocus -= OnGotFocus;
            view.LostFocus -= OnLostFocus;
            view.KeyDown -= OnKeyDown;
            view.KeyUp -= OnKeyUp;
            base.OnDropViewInstance(reactContext, view);
        }

        private void OnGotFocus(object sender, RoutedEventArgs e)
        {
            if (e.OriginalSource == sender)
            {
                var view = (ReactControl)sender;
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(new FocusEvent(view.GetTag()));
            }
        }

        private void OnLostFocus(object sender, RoutedEventArgs e)
        {
            if (e.OriginalSource == sender)
            {
                var view = (ReactControl)sender;
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(new BlurEvent(view.GetTag()));
            }
        }

#if WINDOWS_UWP
        private void OnKeyDown(object sender, KeyRoutedEventArgs e)
#else
        private void OnKeyDown(object sender, KeyEventArgs e)
#endif
        {
            if (sender == e.OriginalSource)
            {
                var view = (ReactControl)sender;
                var keyCode = e.Key.GetKeyCode();
                e.Handled = view.HandledKeyDownKeys?.Contains(keyCode) ?? false;
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                        new KeyEvent(
                            KeyEvent.KeyDownEventString,
                            view.GetTag(),
                            e.Key));
            }
        }

#if WINDOWS_UWP
        private void OnKeyUp(object sender, KeyRoutedEventArgs e)
#else
        private void OnKeyUp(object sender, KeyEventArgs e)
#endif
        {
            if (sender == e.OriginalSource)
            {
                var view = (ReactControl)sender;
                var keyCode = e.Key.GetKeyCode();
                e.Handled = view.HandledKeyUpKeys?.Contains(keyCode) ?? false;
                view.GetReactContext()
                    .GetNativeModule<UIManagerModule>()
                    .EventDispatcher
                    .DispatchEvent(
                        new KeyEvent(
                            KeyEvent.KeyUpEventString,
                            view.GetTag(),
                            e.Key));
            }
        }
    }
}
