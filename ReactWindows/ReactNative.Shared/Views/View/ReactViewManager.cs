using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using System.Collections.Generic;
using System.Linq;
#if WINDOWS_UWP
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
#endif

namespace ReactNative.Views.View
{
    /// <summary>
    /// View manager for React view instances.
    /// </summary>
    public class ReactViewManager : ViewParentManager<ReactViewControl>
    {
        private const int FocusCommand = 1;
        private const int BlurCommand = 2;

        private enum Radius
        {
            All,
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
        }

        /// <summary>
        /// Default brush for the view borders.
        /// </summary>
        protected static readonly Brush s_defaultBorderBrush = new SolidColorBrush(Colors.Black);

        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTView";
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
        /// Checks if the Canvas has a Border already.
        /// </summary>
        protected bool HasBorder(ReactViewControl view)
        {
            return view.Border != null;
        }

        /// <summary>
        /// Adds a Border to a Canvas if it hasn't been added already.
        /// </summary>
        protected Border GetOrCreateBorder(ReactViewControl view)
        {
            if (view.Border == null)
            {
                view.Border = new Border { BorderBrush = s_defaultBorderBrush };

                // Layout animations bypass SetDimensions, hence using XAML bindings.

                view.Border.SetBinding(FrameworkElement.WidthProperty, new Binding
                {
                    Source = view,
                    Path = new PropertyPath("Width")
                });

                view.Border.SetBinding(FrameworkElement.HeightProperty, new Binding
                {
                    Source = view,
                    Path = new PropertyPath("Height")
                });
            }

            return view.Border;
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override ReactViewControl CreateViewInstance(ThemedReactContext reactContext)
        {
            return new ReactViewControl();
        }

        /// <summary>
        /// Sets whether or not the view is an accessibility element.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="accessible">A flag indicating whether or not the view is an accessibility element.</param>
        [ReactProp("accessible")]
        public void SetAccessible(ReactViewControl view, bool accessible)
        {
            // TODO: #557 Provide implementation for View's accessible prop

            // We need to have this stub for this prop so that Views which
            // specify the accessible prop aren't considered to be layout-only.
            // The proper implementation is still to be determined.
        }

        /// <summary>
        /// Set the pointer events handling mode for the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="pointerEventsValue">The pointerEvents mode.</param>
        [ReactProp("pointerEvents")]
        public void SetPointerEvents(ReactViewControl view, string pointerEventsValue)
        {
            var pointerEvents = EnumHelpers.ParseNullable<PointerEvents>(pointerEventsValue) ?? PointerEvents.Auto;
            view.SetPointerEvents(pointerEvents);
        }

        /// <summary>
        /// Sets the border radius of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="index">The property index.</param>
        /// <param name="radius">The border radius value.</param>
        [ReactPropGroup(
            ViewProps.BorderRadius,
            ViewProps.BorderTopLeftRadius,
            ViewProps.BorderTopRightRadius,
            ViewProps.BorderBottomLeftRadius,
            ViewProps.BorderBottomRightRadius)]
        public void SetBorderRadius(ReactViewControl view, int index, double radius)
        {
            var border = GetOrCreateBorder(view);
            var cornerRadius = border.CornerRadius == null ? new CornerRadius() : border.CornerRadius;

            switch ((Radius)index)
            {
                case Radius.All:
                    cornerRadius = new CornerRadius(radius);
                    break;
                case Radius.TopLeft:
                    cornerRadius.TopLeft = radius;
                    break;
                case Radius.TopRight:
                    cornerRadius.TopRight = radius;
                    break;
                case Radius.BottomLeft:
                    cornerRadius.BottomLeft = radius;
                    break;
                case Radius.BottomRight:
                    cornerRadius.BottomRight = radius;
                    break;
            }

            border.CornerRadius = cornerRadius;
        }

        /// <summary>
        /// Sets the background color of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(
            ViewProps.BackgroundColor,
            CustomType = "Color",
            DefaultUInt32 = ColorHelpers.Transparent)]
        public void SetBackgroundColor(ReactViewControl view, uint color)
        {
            var border = GetOrCreateBorder(view);
            border.Background = new SolidColorBrush(ColorHelpers.Parse(color));
        }

        /// <summary>
        /// Set the border color of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="color">The color hex code.</param>
        [ReactProp("borderColor", CustomType = "Color")]
        public void SetBorderColor(ReactViewControl view, uint? color)
        {
            var border = GetOrCreateBorder(view);
            border.BorderBrush = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : s_defaultBorderBrush;
        }

        /// <summary>
        /// Sets the border thickness of the view.
        /// </summary>
        /// <param name="view">The view panel.</param>
        /// <param name="index">The property index.</param>
        /// <param name="width">The border width in pixels.</param>
        [ReactPropGroup(
            ViewProps.BorderWidth,
            ViewProps.BorderLeftWidth,
            ViewProps.BorderRightWidth,
            ViewProps.BorderTopWidth,
            ViewProps.BorderBottomWidth)]
        public void SetBorderWidth(ReactViewControl view, int index, double width)
        {
            var border = GetOrCreateBorder(view);
            border.SetBorderWidth(ViewProps.BorderSpacingTypes[index], width);
        }

        /// <summary>
        /// Sets whether the view is collapsible.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="collapsible">The flag.</param>
        [ReactProp(ViewProps.Collapsible)]
        public void SetCollapsible(ReactViewControl view, bool collapsible)
        {
            // no-op: it's here only so that "collapsable" property is exported to JS. The value is actually
            // handled in NativeViewHierarchyOptimizer
        }

        /// <summary>
        /// Sets whether the view is a tab stop.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="isTabStop">
        /// <code>true</code> if the view is a tab stop, otherwise <code>false</code>.
        /// </param>
        [ReactProp(ViewProps.IsTabStop)]
        public void SetIsTabStop(ReactViewControl view, bool isTabStop)
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
        /// Sets the keys to handle during the <see cref="UIElement.KeyDownEvent"/>. 
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="keys">The keys.</param>
        [ReactProp("handledKeyDownKeys")]
        public void SetHandledKeyDownKeys(ReactViewControl view, int[] keys)
        {
            view.HandledKeyDownKeys = keys;
        }

        /// <summary>
        /// Sets the keys to handle during the <see cref="UIElement.KeyUpEvent"/>. 
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="keys">The keys.</param>
        [ReactProp("handledKeyUpKeys")]
        public void SetHandledKeyUpKeys(ReactViewControl view, int[] keys)
        {
            view.HandledKeyUpKeys = keys;
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(ReactViewControl parent, DependencyObject child, int index)
        {
            if (HasBorder(parent))
            {
                index++;
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
        public override DependencyObject GetChildAt(ReactViewControl parent, int index)
        {
            if (HasBorder(parent))
            {
                index++;
            }

            return parent.Children[index];
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(ReactViewControl parent)
        {
            var count = parent.Children.Count;

            if (HasBorder(parent))
            {
                count--;
            }

            return count;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(ReactViewControl parent)
        {
            if (HasBorder(parent))
            {
                for (var i = parent.Children.Count - 1; i > 0; i--)
                {
                    parent.Children.RemoveAt(i);
                }
            }
            else
            {
                parent.Children.Clear();
            }
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(ReactViewControl parent, int index)
        {
            if (HasBorder(parent))
            {
                index++;
            }

            parent.Children.RemoveAt(index);
        }

        /// <summary>
        /// Receive a command from JavaScript.
        /// </summary>
        /// <param name="view">
        /// The view instance that should receive the command.
        /// </param>
        /// <param name="commandId">Identifer for the command.</param>
        /// <param name="args">Optional arguments for the command.</param>
        public override void ReceiveCommand(ReactViewControl view, int commandId, JArray args)
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
        public override void OnDropViewInstance(ThemedReactContext reactContext, ReactViewControl view)
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
                var view = (ReactViewControl)sender;
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
                var view = (ReactViewControl)sender;
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
                var view = (ReactViewControl)sender;
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
                var view = (ReactViewControl)sender;
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
