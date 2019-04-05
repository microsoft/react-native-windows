// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

namespace ReactNative.UIManager
{
    /// <summary>
    /// View manager for React root view components.
    /// </summary>
    public class RootViewManager : PanelViewParentManager<SizeMonitoringCanvas>
    {
        /// <summary>
        /// The name of the React root view.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RootView";
            }
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="RootViewManager"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, SizeMonitoringCanvas view)
        {
            view.RemoveSizeChanged();
        }

        /// <summary>
        /// Used to install custom event emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        /// <remarks>
        /// Intentionally skipping call to base method because we don't care
        /// about pointer enter/leave events on the root view.
        /// </remarks>
        protected override void AddEventEmitters(ThemedReactContext reactContext, SizeMonitoringCanvas view)
        {
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="SizeMonitoringCanvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override SizeMonitoringCanvas CreateViewInstance(ThemedReactContext reactContext)
        {
            return new SizeMonitoringCanvas();
        }
    }
}