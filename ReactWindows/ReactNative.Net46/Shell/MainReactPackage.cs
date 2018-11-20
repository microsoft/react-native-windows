// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Animated;
using ReactNative.Bridge;
using ReactNative.Modules.AppState;
using ReactNative.Modules.Clipboard;
using ReactNative.Modules.Core;
using ReactNative.Modules.Dialog;
using ReactNative.Modules.I18N;
using ReactNative.Modules.Image;
using ReactNative.Modules.Launch;
using ReactNative.Modules.NetInfo;
using ReactNative.Modules.Network;
using ReactNative.Modules.Storage;
using ReactNative.Modules.WebSocket;
using ReactNative.UIManager;
using ReactNative.Views.ControlView;
using ReactNative.Views.Image;
using ReactNative.Views.Picker;
using ReactNative.Views.Progress;
using ReactNative.Views.Scroll;
using ReactNative.Views.Slider;
using ReactNative.Views.Text;
using ReactNative.Views.TextInput;
using ReactNative.Views.View;
using ReactNative.Views.Web;
using System.Collections.Generic;

namespace ReactNative.Shell
{
    /// <summary>
    /// Package defining basic modules and view managers.
    /// </summary>
    public class MainReactPackage : IReactPackage
    {
        /// <summary>
        /// Creates the list of native modules to register with the react
        /// instance.
        /// </summary>
        /// <param name="reactContext">The React application context.</param>
        /// <returns>The list of native modules.</returns>
        public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
        {
            return new List<INativeModule>
            {
                new AppStateModule(reactContext),
                new AsyncStorageModule(),
                //new CameraRollManager(reactContext),
                new ClipboardModule(),
                new DialogModule(reactContext),
                new ImageLoaderModule(),
                new I18NModule(reactContext),
                new LauncherModule(reactContext),
                //new LocationModule(reactContext),
                new NativeAnimatedModule(reactContext),
                new NetworkingModule(reactContext),
                new NetInfoModule(reactContext),
                //new StatusBarModule(),
                //new VibrationModule(),
                new WebSocketModule(reactContext),
            };
        }

        /// <summary>
        /// Creates the list of view managers that should be registered with
        /// the <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="reactContext">The React application context.</param>
        /// <returns>The list of view managers.</returns>
        public IReadOnlyList<IViewManager> CreateViewManagers(
            ReactContext reactContext)
        {
            return new List<IViewManager>
            {
                new ReactSimpleTextViewManager(),
                //new ReactFlipViewManager(),
                new ReactImageManager(),
                new ReactProgressBarViewManager(),
                //new ReactProgressRingViewManager(),
                new ReactPickerManager(),
                new ReactRunManager(),
                ////new RecyclerViewBackedScrollViewManager(),
                new ReactScrollViewManager(),
                new ReactSliderManager(),
                //new ReactSplitViewManager(),
                //new ReactSwitchManager(),
                new ReactPasswordBoxManager(),
                new ReactTextInputManager(),
                new ReactTextViewManager(),
                new ReactControlManager(),
                new ReactViewManager(),
                new ReactSpanViewManager(),
                ////new SwipeRefreshLayoutManager(),
                new ReactWebViewManager(),
            };
        }
    }
}
