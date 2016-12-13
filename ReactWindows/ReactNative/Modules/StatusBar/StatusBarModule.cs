using ReactNative.Bridge;
using ReactNative.UIManager;
using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Foundation.Metadata;
using Windows.UI;
using Windows.UI.Core;

namespace ReactNative.Modules.StatusBar
{
    /// <summary>
    /// A module that allows JS to set StatusBar properties.
    /// </summary>
    public class StatusBarModule : NativeModuleBase
    {
        private readonly IStatusBar _statusBar;

        /// <summary>
        /// Instantiates the <see cref="StatusBarModule"/>.
        /// </summary>
        internal StatusBarModule() 
            : this(GetStatusBar())
        {
        }

        /// <summary>
        /// Instantiates the <see cref="StatusBarModule"/>.
        /// </summary>
        /// <param name="statusBar">The status bar instance.</param>
        internal StatusBarModule(IStatusBar statusBar)
        {
            if (statusBar == null)
                throw new ArgumentNullException(nameof(statusBar));

            _statusBar = statusBar;
        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "StatusBarManager";
            }
        }

        /// <summary>
        /// Hide or show StatusBar.
        /// </summary>
        /// <param name="hide">Hide or show StatusBar.</param>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "React method must return void.")]
        [ReactMethod]
#pragma warning disable AvoidAsyncVoid
        public async void setHidden(bool hide)
#pragma warning restore AvoidAsyncVoid
        {
            RunOnDispatcher(async () =>
            {
                if (hide)
                {
                    await _statusBar.HideAsync().AsTask().ConfigureAwait(false);
                }
                else
                {
                    await _statusBar.ShowAsync().AsTask().ConfigureAwait(false);
                }
            });
        }

        /// <summary>
        /// Set StatusBar background color.
        /// </summary>
        /// <param name="color">RGB color.</param>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "React method must return void.")]
        [ReactMethod]
#pragma warning disable AvoidAsyncVoid
        public async void setColor(uint? color)
#pragma warning restore AvoidAsyncVoid
        {
            RunOnDispatcher(() =>
            {
                _statusBar.BackgroundColor = !color.HasValue
                    ? default(Color?)
                    : ColorHelpers.Parse(color.Value);
            });
        }

        /// <summary>
        /// Set StatusBar opacity.
        /// </summary>
        /// <param name="translucent">Is StatusBar translucent.</param>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "React method must return void.")]
        [ReactMethod]
#pragma warning disable AvoidAsyncVoid
        public async void setTranslucent(bool translucent)
#pragma warning restore AvoidAsyncVoid
        {
            RunOnDispatcher(() =>
            {
                _statusBar.BackgroundOpacity = translucent ? 0.5 : 1;
            });
        }

        /// <summary>
        /// Create default StatusBar.
        /// </summary>
        private static IStatusBar GetStatusBar()
        {
            if (ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
            {
                return new MobileStatusBar();
            }
            else
            {
                return new NopStatusBar();
            }
        }

        /// <summary>
        /// Run action on UI thread.
        /// </summary>
        /// <param name="action">The action.</param>

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "Fire-and-forget method.")]
#pragma warning disable AvoidAsyncVoid
        private static async void RunOnDispatcher(DispatchedHandler action)
#pragma warning restore AvoidAsyncVoid
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, action).AsTask().ConfigureAwait(false);
        }
    }
}
