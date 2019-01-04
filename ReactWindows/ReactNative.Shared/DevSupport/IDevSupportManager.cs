// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.DevSupport;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.DevSupport
{
    /// <summary>
    /// Interface for accessing and interacting with development features.
    /// In Debug builds, use <see cref="DevSupportManager"/>; for release, use
    /// <see cref="DisabledDevSupportManager"/>.
    /// </summary>
    public interface IDevSupportManager
    {
        /// <summary>
        /// The developer settings.
        /// </summary>
        IDeveloperSettings DevSettings { get; }

        /// <summary>
        /// The cached JavaScript bundle.
        /// </summary>
        string DownloadedJavaScriptBundleFile { get; }

        /// <summary>
        /// Enables or disables the instance.
        /// </summary>
        bool IsEnabled { get; set; }

        /// <summary>
        /// Enables or disables remote debugging.
        /// </summary>
        bool IsRemoteDebuggingEnabled { get; set; }

        /// <summary>
        /// Enables or disables the progress dialog.
        /// </summary>
        bool IsProgressDialogEnabled { get; set; }

        /// <summary>
        /// The source map URL.
        /// </summary>
        string SourceMapUrl { get; }

        /// <summary>
        /// The source URL.
        /// </summary>
        string SourceUrl { get; }

        /// <summary>
        /// The source URL used during remote debugging.
        /// </summary>
        string JavaScriptBundleUrlForRemoteDebugging { get; }

        /// <summary>
        /// Called before the dev options dialog is opened
        /// </summary>
        event Action BeforeShowDevOptionsDialog;

        /// <summary>
        /// Handle a native exception.
        /// </summary>
        /// <param name="exception">The exception.</param>
        void HandleException(Exception exception);

        /// <summary>
        /// Handles reloading the JavaScript bundle.
        /// </summary>
        void HandleReloadJavaScript();

        /// <summary>
        /// Handles reloading the JavaScript bundle.
        /// It is called with the react instance manager lock locked
        /// </summary>
        /// <param name="token">A token to cancel the operation.</param>
        /// <returns>A task to await the result.</returns>
        Task<ReactContext> CreateReactContextFromPackagerAsync(CancellationToken token);

        /// <summary>
        /// Checks if an up-to-date JavaScript bundle is ready.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the cached bundle is newer than the date the
        /// application was installed, otherwise <code>false</code>.
        /// </returns>
        bool HasUpToDateBundleInCache();

        /// <summary>
        /// Dismisses the red box exception dialog.
        /// </summary>
        void HideRedboxDialog();

        /// <summary>
        /// Checks if the packager is running.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>
        /// <code>true</code> if the packager is running, otherwise <code>false</code>.
        /// </returns>
        Task<bool> IsPackagerRunningAsync(CancellationToken token);

        /// <summary>
        /// Notify when a new React context is created.
        /// </summary>
        /// <param name="context">The React context.</param>
        void OnNewReactContextCreated(ReactContext context);

        /// <summary>
        /// Notify when a new React context is destroyed.
        /// </summary>
        /// <param name="context">The React context.</param>
        void OnReactContextDestroyed(ReactContext context);

        /// <summary>
        /// Reload developer settings.
        /// </summary>
        void ReloadSettings();

        /// <summary>
        /// Show the developer options dialog.
        /// </summary>
        void ShowDevOptionsDialog();

        /// <summary>
        /// Display a JavaScript error.
        /// </summary>
        /// <param name="title">The error message.</param>
        /// <param name="details">The error stack trace.</param>
        /// <param name="exceptionId">An identifier for the exception.</param>
        void ShowNewJavaScriptError(string title, JArray details, int exceptionId);

        /// <summary>
        /// Display a native exception.
        /// </summary>
        /// <param name="message">The error message.</param>
        /// <param name="ex">The thrown exception.</param>
        void ShowNewNativeError(string message, Exception ex);

        /// <summary>
        /// Update the details of a JavaScript exception.
        /// </summary>
        /// <param name="title">The error message.</param>
        /// <param name="details">The error stack trace.</param>
        /// <param name="exceptionId">An identifier for the exception.</param>
        void UpdateJavaScriptError(string title, JArray details, int exceptionId);
    }
}
