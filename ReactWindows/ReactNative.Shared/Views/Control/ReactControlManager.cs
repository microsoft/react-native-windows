using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using System.Collections.Generic;
using System.Linq;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
#endif

namespace ReactNative.Views.Control
{
    /// <summary>
    /// View manager for React control instances.
    /// </summary>
    public class ReactControlManager : ViewParentManager<ReactControl>
    {
        private const int FocusCommand = 1;
        private const int BlurCommand = 2;

        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name
        {
            get
            {
                return "WindowsControl";
            }
        }

        /// <summary>
        /// The commands.
        /// </summary>
        public override IReadOnlyDictionary<string, object> CommandsMap
        {
            get
            {
                return new Dictionary<string, object>
                {
                    { "focus", FocusCommand },
                    { "blur", BlurCommand },
                };
            }
        }

        /// <summary>
        /// Exported view constants.
        /// </summary>
        public override IReadOnlyDictionary<string, object> ExportedViewConstants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    { "Keys", KeyHelpers.GetKeyConstants() },
                };
            }
        }

        /// <summary>
        /// Exported set of bubbling events.
        /// </summary>
        public override IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    {
                        "topFocus",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onFocus" },
                                    { "captured" , "onFocusCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topBlur",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onBlur" },
                                    { "captured" , "onBlurCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topKeyDown",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onKeyDown" },
                                    { "captured" , "onKeyDownCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topKeyUp",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onKeyUp" },
                                    { "captured" , "onKeyUpCapture" }
                                }
                            }
                        }
                    },
                };
            }
        }

        /// <summary>
        /// Sets whether the view is a tab stop.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="isTabStop">
        /// <code>true</code> if the view is a tab stop, otherwise <code>false</code>.
        /// </param>
        [ReactProp("isTabStop")]
        public void SetIsTabStop(ReactControl view, bool isTabStop)
        {
            view.IsTabStop = isTabStop;
            if (isTabStop)
            {
                view.GotFocus += OnGotFocus;
                view.LostFocus += OnLostFocus;
                view.KeyUp += OnKeyUp;
                view.KeyDown += OnKeyDown;
            }
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

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(ReactControl parent, DependencyObject child, int index)
        {
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
            base.OnDropViewInstance(reactContext, view);
            if (view.IsTabStop)
            {
                view.GotFocus -= OnGotFocus;
                view.LostFocus -= OnLostFocus;
                view.KeyDown -= OnKeyDown;
                view.KeyUp -= OnKeyUp;
            }
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
                            keyCode));
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
                            keyCode));
            }
        }

        class FocusEvent : Event
        {
            public FocusEvent(int viewTag)
                : base(viewTag)
            {
            }

            public override string EventName
            {
                get
                {
                    return "topFocus";
                }
            }

            public override bool CanCoalesce
            {
                get
                {
                    return false;
                }
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                var eventData = new JObject
                {
                    { "target", ViewTag },
                };

                eventEmitter.receiveEvent(ViewTag, EventName, eventData);
            }
        }

        class BlurEvent : Event
        {
            public BlurEvent(int viewTag)
                : base(viewTag)
            {
            }

            public override string EventName
            {
                get
                {
                    return "topBlur";
                }
            }

            public override bool CanCoalesce
            {
                get
                {
                    return false;
                }
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                var eventData = new JObject
                {
                    { "target", ViewTag },
                };

                eventEmitter.receiveEvent(ViewTag, EventName, eventData);
            }
        }

        class KeyEvent : Event
        {
            public const string KeyDownEventString = "topKeyDown";
            public const string KeyUpEventString = "topKeyUp";

            private readonly int _key;

            public KeyEvent(string eventName, int viewTag, int key)
                : base(viewTag)
            {
                EventName = eventName;
                _key = key;
            }

            public override string EventName
            {
                get;
            }

            public override bool CanCoalesce
            {
                get
                {
                    return false;
                }
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                var eventData = new JObject
                {
                    { "target", ViewTag },
                    { "key", _key },
                };

                eventEmitter.receiveEvent(ViewTag, EventName, eventData);
            }
        }
    }
}
