// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Flip
{
    class ReactFlipViewManager : ViewParentManager<FlipView>
    {
        private const int SetPage = 1;

        public override string Name
        {
            get
            {
                return "WindowsFlipView";
            }
        }

        public override JObject ViewCommandsMap
        {
            get
            {
                return new JObject
                {
                    { "setPage", SetPage },
                };
            }
        }

        public override JObject CustomDirectEventTypeConstants
        {
            get
            {
                return new JObject
                {
                    {
                        "topSelectionChange",
                        new JObject
                        {
                            { "registrationName", "onSelectionChange" },
                        }
                    },
                };
            }
        }

        [ReactProp("alwaysAnimate", DefaultBoolean = true)]
        public void SetAlwaysAnimate(FlipView view, bool alwaysAnimate)
        {
            view.UseTouchAnimationsForAllNavigation = alwaysAnimate;
        }

        [ReactProp("backgroundColor")]
        public void SetBackgroundColor(FlipView view, uint? color)
        {
            view.Background = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        public override void AddView(FlipView parent, DependencyObject child, int index)
        {
            parent.Items.Insert(index, child);
        }

        public override DependencyObject GetChildAt(FlipView parent, int index)
        {
            return (DependencyObject)parent.Items[index];
        }

        public override int GetChildCount(FlipView parent)
        {
            return parent.Items.Count;
        }

        public override void RemoveAllChildren(FlipView parent)
        {
            parent.Items.Clear();
        }

        public override void RemoveChildAt(FlipView parent, int index)
        {
            parent.Items.RemoveAt(index);
        }

        public override void OnDropViewInstance(ThemedReactContext reactContext, FlipView view)
        {
            base.OnDropViewInstance(reactContext, view);
            view.SelectionChanged -= OnSelectionChanged;
        }

        public override async void ReceiveCommand(FlipView view, int commandId, JArray args)
        {
            switch (commandId)
            {
                case SetPage:
                    // TODO: (#328) Fix issue with `setPage` on mount
                    await Task.Yield();
                    view.SelectionChanged -= OnSelectionChanged;
                    view.SelectedIndex = args.First.Value<int>();
                    view.SelectionChanged += OnSelectionChanged;
                    break;
            }
        }

        protected override FlipView CreateViewInstance(ThemedReactContext reactContext)
        {
            return new FlipView();
        }

        protected override void AddEventEmitters(ThemedReactContext reactContext, FlipView view)
        {
            base.AddEventEmitters(reactContext, view);
            view.SelectionChanged += OnSelectionChanged;
        }

        private void OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var flipView = (FlipView)sender;
            flipView.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new SelectionChangedEvent(
                        flipView.GetTag(),
                        flipView.SelectedIndex));
        }

        class SelectionChangedEvent : Event
        {
            private readonly int _position;

            public SelectionChangedEvent(int viewTag, int position)
                : base(viewTag)
            {
                _position = position;
            }

            public override string EventName
            {
                get
                {
                    return "topSelectionChange";
                }
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                var eventData = new JObject
                {
                    { "position", _position },
                };

                eventEmitter.receiveEvent(ViewTag, EventName, eventData);
            }
        }
    }
}
