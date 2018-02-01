using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Bridge.Queue;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Native module to allow JavaScript to create and update native views.
    /// </summary>
    public partial class UIManagerModule : ReactContextNativeModuleBase, ILifecycleEventListener, IOnBatchCompleteListener
    {
        private const int RootViewTagIncrement = 10;

        private readonly UIImplementation _uiImplementation;
        private readonly IReadOnlyDictionary<string, object> _moduleConstants;
        private readonly IReadOnlyDictionary<string, object> _customDirectEvents;
        private readonly EventDispatcher _eventDispatcher;
        private readonly IActionQueue _layoutActionQueue;

        private int _batchId;
        private int _nextRootTag = 1;

        /// <summary>
        /// Instantiates a <see cref="UIManagerModule"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="viewManagers">The view managers.</param>
        /// <param name="uiImplementationProvider">The UI implementation provider.</param>
        /// <param name="layoutActionQueue">The layout action queue.</param>
        public UIManagerModule(
            ReactContext reactContext,
            IReadOnlyList<IViewManager> viewManagers,
            UIImplementationProvider uiImplementationProvider,
            IActionQueue layoutActionQueue)
            : base(reactContext, layoutActionQueue)
        {
            if (viewManagers == null)
                throw new ArgumentNullException(nameof(viewManagers));
            if (uiImplementationProvider == null)
                throw new ArgumentNullException(nameof(uiImplementationProvider));
            if (layoutActionQueue == null)
                throw new ArgumentNullException(nameof(layoutActionQueue));

            _eventDispatcher = new EventDispatcher(reactContext);
            _uiImplementation = uiImplementationProvider.Create(reactContext, viewManagers, _eventDispatcher);
            var customDirectEvents = new Dictionary<string, object>();
            _customDirectEvents = customDirectEvents;
            _moduleConstants = CreateConstants(viewManagers, null, customDirectEvents);
            _layoutActionQueue = layoutActionQueue;
            reactContext.AddLifecycleEventListener(this);
        }

        /// <summary>
        /// An event fired before the UIManager dispatches view updates.
        /// </summary>
        public event EventHandler DispatchingViewUpdates;

        /// <summary>
        /// The name of the module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "UIManager";
            }
        }

        /// <summary>
        /// The constants exported by this module.
        /// </summary>
        public override IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return _moduleConstants;
            }
        }

        /// <summary>
        /// The event dispatcher for the module.
        /// </summary>
        public EventDispatcher EventDispatcher
        {
            get
            {
                return _eventDispatcher;
            }
        }

        /// <summary>
        /// The UI implementation.
        /// </summary>
        public UIImplementation UIImplementation
        {
            get
            {
                return _uiImplementation;
            }
        }

        /// <summary>
        /// Registers a new root view.
        /// </summary>
        /// <param name="rootView">The root view instance.</param>
        /// <returns>The root view tag.</returns>
        /// <remarks>
        /// JavaScript can use the returned tag with to add or remove children 
        /// to this view through <see cref="manageChildren(int, int[], int[], int[], int[], int[])"/>.
        /// </remarks>
        public int AddMeasuredRootView(ReactRootView rootView)
        {
            // Called on main dispatcher thread
            DispatcherHelpers.AssertOnDispatcher();

            var tag = _nextRootTag;
            _nextRootTag += RootViewTagIncrement;

            var context = new ThemedReactContext(Context);

            DispatcherHelpers.RunOnDispatcher(rootView.Dispatcher, () =>
            {
                var width = rootView.ActualWidth;
                var height = rootView.ActualHeight;

                _layoutActionQueue.Dispatch(() =>
                {
                    _uiImplementation.RegisterRootView(rootView, tag, width, height, context);
                });

                var resizeCount = 0;

                rootView.SetOnSizeChangedListener((sender, args) =>
                {
                    var currentCount = ++resizeCount;
                    var newWidth = args.NewSize.Width;
                    var newHeight = args.NewSize.Height;

                    _layoutActionQueue.Dispatch(() =>
                    {
                        if (currentCount == resizeCount)
                        {
                            _layoutActionQueue.AssertOnThread();
                            _uiImplementation.UpdateRootNodeSize(tag, newWidth, newHeight);
                        }
                    });
                });
 
            }, true); // Allow inlining

            return tag;
        }

        /// <summary>
        /// Schedule a block to be executed on the UI thread. Useful if you need to execute
        /// view logic after all currently queued view updates have completed.
        /// </summary>
        /// <param name="block">The UI block.</param>
        public void AddUIBlock(IUIBlock block)
        {
            _uiImplementation.AddUIBlock(block);
        }

        /// <summary>
        /// Schedule a block to be executed on the UI thread. Useful if you need to execute
        /// need view logic before all currently queued view updates have completed.
        /// </summary>
        /// <param name="block">The UI block.</param>
        public void PrependUIBlock(IUIBlock block)
        {
            _uiImplementation.PrependUIBlock(block);
        }

        /// <summary>
        /// Resolves direct event name exposed to JavaScript from the one known
        /// to the native implementation.
        /// </summary>
        /// <param name="eventName">The native event name.</param>
        /// <returns>The direct event name.</returns>
        public string ResolveCustomEventName(string eventName)
        {
            var value = default(object);
            if (!_customDirectEvents.TryGetValue(eventName, out value))
            {
                return eventName;
            }

            var customEventType = value as IDictionary<string, object>;
            if (customEventType == null ||
                !customEventType.TryGetValue("registrationName", out value))
            {
                return eventName;
            }

            var registrationName = value as string;
            return registrationName ?? eventName;
        }

        #region React Methods

        /// <summary>
        /// Removes the root view.
        /// </summary>
        /// <param name="rootViewTag">The root view tag.</param>
        [ReactMethod]
        public void removeRootView(int rootViewTag)
        {
            _uiImplementation.RemoveRootView(rootViewTag);
        }

        /// <summary>
        /// Creates a view.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="className">The class name.</param>
        /// <param name="rootViewTag">The root view tag.</param>
        /// <param name="props">The properties.</param>
        [ReactMethod]
        public void createView(int tag, string className, int rootViewTag, JObject props)
        {
            _uiImplementation.CreateView(tag, className, rootViewTag, props);
        }

        /// <summary>
        /// Updates a view.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="className">The class name.</param>
        /// <param name="props">The properties.</param>
        [ReactMethod]
        public void updateView(int tag, string className, JObject props)
        {
            _uiImplementation.UpdateView(tag, className, props);
        }

        /// <summary>
        /// Manage the children of a view.
        /// </summary>
        /// <param name="viewTag">The view tag of the parent view.</param>
        /// <param name="moveFrom">
        /// A list of indices in the parent view to move views from.
        /// </param>
        /// <param name="moveTo">
        /// A list of indices in the parent view to move views to.
        /// </param>
        /// <param name="addChildTags">
        /// A list of tags of views to add to the parent.
        /// </param>
        /// <param name="addAtIndexes">
        /// A list of indices to insert the child tags at.
        /// </param>
        /// <param name="removeFrom">
        /// A list of indices to permanently remove. The memory for the
        /// corresponding views and data structures should be reclaimed.
        /// </param>
        [ReactMethod]
        public void manageChildren(
            int viewTag,
            int[] moveFrom,
            int[] moveTo,
            int[] addChildTags,
            int[] addAtIndexes,
            int[] removeFrom)
        {
            _uiImplementation.ManageChildren(
                    viewTag,
                    moveFrom,
                    moveTo,
                    addChildTags,
                    addAtIndexes,
                    removeFrom);
        }

        /// <summary>
        /// Interface for fast tracking the initial adding of views.Children 
        /// view tags are assumed to be in order.
        /// </summary>
        /// <param name="viewTag">The view tag of the parent view.</param>
        /// <param name="childrenTags">
        /// An array of tags to add to the parent in order.
        /// </param>
        [ReactMethod]
        public void setChildren(
            int viewTag,
            int[] childrenTags)
        {
            _uiImplementation.SetChildren(viewTag, childrenTags);
        }

        /// <summary>
        /// Replaces the view specified by the <paramref name="oldTag"/> with
        /// the view specified by <paramref name="newTag"/> within
        /// <paramref name="oldTag"/>'s parent. This resolves to a simple 
        /// <see cref="manageChildren(int, int[], int[], int[], int[], int[])"/>
        /// call, but React does not have enough information in JavaScript to
        /// formulate it itself.
        /// </summary>
        /// <param name="oldTag">The old view tag.</param>
        /// <param name="newTag">The new view tag.</param>
        [ReactMethod]
        public void replaceExistingNonRootView(int oldTag, int newTag)
        {
            _uiImplementation.ReplaceExistingNonRootView(oldTag, newTag);
        }

        /// <summary>
        /// Method which takes a container tag and then releases all subviews
        /// for that container upon receipt.
        /// </summary>
        /// <param name="containerTag">The container tag.</param>
        [ReactMethod]
        public void removeSubviewsFromContainerWithID(int containerTag)
        {
            _uiImplementation.RemoveSubViewsFromContainerWithId(containerTag);
        }

        /// <summary>
        /// Determines the location on screen, width, and height of the given
        /// view and returns the values via an asynchronous callback.
        /// </summary>
        /// <param name="reactTag">The view tag to measure.</param>
        /// <param name="callback">The callback.</param>
        [ReactMethod]
        public void measure(int reactTag, ICallback callback)
        {
            _uiImplementation.Measure(reactTag, callback);
        }

        /// <summary>
        /// Determines the width, height, and location relative to the window
        /// of the given view and returns the values via an asynchronous callback.
        /// </summary>
        /// <param name="reactTag">The view tag to measure.</param>
        /// <param name="callback">The callback.</param>
        [ReactMethod]
        public void measureInWindow(int reactTag, ICallback callback)
        {
            _uiImplementation.MeasureInWindow(reactTag, callback);
        }

        /// <summary>
        /// Measure the view specified by <paramref name="tag"/> to the given
        /// <paramref name="ancestorTag"/>. This means that the returned x, y
        /// are relative to the origin x, y of the ancestor view.
        /// </summary>
        /// <param name="tag">The view tag.</param>
        /// <param name="ancestorTag">The ancestor tag.</param>
        /// <param name="errorCallback">The error callback.</param>
        /// <param name="successCallback">The success callback.</param>
        /// <remarks>
        /// NB: Unlike <see cref="measure(int, ICallback)"/>, this will measure
        /// relative to the view layout, not the visible window which can cause
        /// unexpected results when measuring relative to things like scroll
        /// views that can have offset content on the screen.
        /// </remarks>
        [ReactMethod]
        public void measureLayout(
            int tag,
            int ancestorTag,
            ICallback errorCallback,
            ICallback successCallback)
        {
            _uiImplementation.MeasureLayout(tag, ancestorTag, errorCallback, successCallback);
        }

        /// <summary>
        /// Like <see cref="measure(int, ICallback)"/> and
        /// <see cref="measureLayout(int, int, ICallback, ICallback)"/> but
        /// measures relative to the immediate parent.
        /// </summary>
        /// <param name="tag">The view tag to measure.</param>
        /// <param name="errorCallback">The error callback.</param>
        /// <param name="successCallback">The success callback.</param>
        /// <remarks>
        /// NB: Unlike <see cref="measure(int, ICallback)"/>, this will measure
        /// relative to the view layout, not the visible window which can cause
        /// unexpected results when measuring relative to things like scroll
        /// views that can have offset content on the screen.
        /// </remarks>
        [ReactMethod]
        public void measureLayoutRelativeToParent(
            int tag,
            ICallback errorCallback,
            ICallback successCallback)
        {
            _uiImplementation.MeasureLayoutRelativeToParent(tag, errorCallback, successCallback);
        }

        /// <summary>
        /// Find the touch target child native view in the supplied root view
        /// hierarchy, given a React target location.
        /// </summary>
        /// <param name="reactTag">The root tag to traverse.</param>
        /// <param name="point">An array containing target coordinates.</param>
        /// <param name="callback">
        /// Callback invoked with the identified child view React tag and
        /// measurement information. If no view was found, callback will be
        /// invoked with no data.
        /// </param>
        /// <remarks>
        /// This method is currently used only by the Element Inspector dev tool.
        /// </remarks>
        [ReactMethod]
        public void findSubviewIn(
            int reactTag,
            JArray point,
            ICallback callback)
        {
            _uiImplementation.FindSubViewIn(
                    reactTag,
                    point[0].Value<double>(),
                    point[1].Value<double>(),
                    callback);
        }

        /// <summary>
        /// Sets a JavaScript responder for a view.
        /// </summary>
        /// <param name="reactTag">The view ID.</param>
        /// <param name="blockNativeResponder">
        /// Flag to signal if the native responder should be blocked.
        /// </param>
        [ReactMethod]
        public void setJSResponder(int reactTag, bool blockNativeResponder)
        {
            // TODO: (#306) implement JS responder handler
        }

        /// <summary>
        /// Clears the JavaScript responder.
        /// </summary>
        [ReactMethod]
        public void clearJSResponder()
        {
            // TODO: (#306) implement JS responder handler
        }

        /// <summary>
        /// Dispatches a command to the view manager.
        /// </summary>
        /// <param name="reactTag">The tag of the view manager.</param>
        /// <param name="commandId">The command ID.</param>
        /// <param name="commandArgs">The command arguments.</param>
        [ReactMethod]
        public void dispatchViewManagerCommand(int reactTag, int commandId, JArray commandArgs)
        {
            _uiImplementation.DispatchViewManagerCommand(reactTag, commandId, commandArgs);
        }

        /// <summary>
        /// Show a pop-up menu.
        /// </summary>
        /// <param name="reactTag">
        /// The tag of the anchor view (the pop-up menu is displayed next to
        /// this view); this needs to be the tag of a native view (shadow views
        /// cannot be anchors).
        /// </param>
        /// <param name="items">The menu items as an array of strings.</param>
        /// <param name="error">
        /// Callback used if there is an error displaying the menu.
        /// </param>
        /// <param name="success">
        /// Callback used with the position of the selected item as the first
        /// argument, or no arguments if the menu is dismissed.
        /// </param>
        [ReactMethod]
        public void showPopupMenu(int reactTag, string[] items, ICallback error, ICallback success)
        {
            _uiImplementation.ShowPopupMenu(reactTag, items, error, success);
        }

        /// <summary>
        /// Configure an animation to be used for the native layout changes,
        /// and native views creation. The animation will only apply during the
        /// current batch operations.
        /// </summary>
        /// <param name="config">
        /// The configuration of the animation for view addition/removal/update.
        /// </param>
        /// <param name="success">
        /// Callback used when the animation completes, or when the animation
        /// gets interrupted. In this case, the callback parameter will be false.
        /// </param>
        /// <param name="error">
        /// Callback used if there was an error processing the animation.
        /// </param>
        [ReactMethod]
        public void configureNextLayoutAnimation(JObject config, ICallback success, ICallback error)
        {
            _uiImplementation.ConfigureNextLayoutAnimation(config, success, error);
        }

        #endregion

        #region ILifecycleEventListener

        /// <summary>
        /// Called when the host receives the suspend event.
        /// </summary>
        public void OnSuspend()
        {
            _uiImplementation.OnSuspend();
        }

        /// <summary>
        /// Called when the host receives the resume event.
        /// </summary>
        public void OnResume()
        {
            _uiImplementation.OnResume();
        }


        /// <summary>
        /// Called when the host is shutting down.
        /// </summary>
        public void OnDestroy()
        {
            _uiImplementation.OnDestroy();
        }

        #endregion

        #region IOnBatchCompleteListener

        /// <summary>
        /// To implement the transactional requirement, UI changes are only
        /// committed to the actual view hierarchy once a batch of JavaScript
        /// to native calls have been completed.
        /// </summary>
        public void OnBatchComplete()
        {
            var batchId = _batchId++;
            DispatchingViewUpdates?.Invoke(this, new EventArgs());
            _uiImplementation.DispatchViewUpdates(batchId);
        }

        #endregion

        #region NativeModuleBase

        /// <summary>
        /// Called before a <see cref="IReactInstance"/> is disposed.
        /// </summary>
        public override void OnReactInstanceDispose()
        {
            _eventDispatcher.OnReactInstanceDispose();
        }

        #endregion
    }
}
