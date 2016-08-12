using ReactNative.Animated;
using ReactNative.Bridge;
using ReactNative.Modules.AppState;
using ReactNative.Modules.Clipboard;
using ReactNative.Modules.Core;
using ReactNative.Modules.Dialog;
using ReactNative.Modules.Image;
using ReactNative.Modules.Launch;
using ReactNative.Modules.Location;
using ReactNative.Modules.NetInfo;
using ReactNative.Modules.Network;
using ReactNative.Modules.StatusBar;
using ReactNative.Modules.Storage;
using ReactNative.Modules.Vibration;
using ReactNative.Modules.WebSocket;
using ReactNative.UIManager;
using ReactNative.Views.Flip;
using ReactNative.Views.Image;
using ReactNative.Views.Picker;
using ReactNative.Views.Progress;
using ReactNative.Views.Scroll;
using ReactNative.Views.Split;
using ReactNative.Views.Switch;
using ReactNative.Views.Text;
using ReactNative.Views.TextInput;
using ReactNative.Views.View;
using ReactNative.Views.Web;
using System;
using System.Collections.Generic;

namespace ReactNative.Shell
{
    /// <summary>
    /// Package defining basic modules and view managers.
    /// </summary>
    public class MainReactPackage : IReactPackage
    {
        private readonly IImageCache _imageCache;
        private readonly IUriLoader _uriLoader;

        /// <summary>
        /// Instantiates the <see cref="MainReactPackage"/>. 
        /// </summary>
        public MainReactPackage()
            : this(new DefaultUriLoader())
        {
        }

        private MainReactPackage(IUriLoader uriLoader)
            : this(new RefCountImageCache(uriLoader), uriLoader)
        {
        }

        /// <summary>
        /// Instantiates the <see cref="MainReactPackage"/>. 
        /// </summary>
        /// <param name="imageCache">The image cache.</param>
        /// <param name="uriLoader">The URI loader.</param>
        public MainReactPackage(IImageCache imageCache, IUriLoader uriLoader)
        {
            _imageCache = imageCache;
            _uriLoader = uriLoader;
        }

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
                new ImageLoaderModule(_imageCache, _uriLoader),
                new LauncherModule(),
                new LocationModule(reactContext),
                new NativeAnimatedModule(reactContext),
                new NetworkingModule(reactContext),
                new NetInfoModule(reactContext),
                new StatusBarModule(),
                new VibrationModule(),
                new WebSocketModule(reactContext),
            };
        }

        /// <summary>
        /// Creates the list of JavaScript modules to register with the 
        /// React instance. 
        /// </summary>
        /// <returns>The list of JavaScript modules.</returns>
        public IReadOnlyList<Type> CreateJavaScriptModulesConfig()
        {
            return new List<Type>(0);
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
                new ReactFlipViewManager(),
                new ReactImageManager(_imageCache),
                new ReactProgressBarViewManager(),
                new ReactProgressRingViewManager(),
                new ReactPickerManager(),
                new ReactRunManager(),
                //new RecyclerViewBackedScrollViewManager(),
                new ReactScrollViewManager(),
                new ReactSplitViewManager(),
                new ReactSwitchManager(),
                new ReactPasswordBoxManager(),
                new ReactTextInputManager(),
                new ReactTextViewManager(),
                new ReactViewManager(),
                new ReactSpanViewManager(),
                //new SwipeRefreshLayoutManager(),
                new ReactWebViewManager(),
            };
        }
    }
}