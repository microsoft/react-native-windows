using System;
using System.Threading;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace ReactUWPTestApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public static bool UseRNTester
        { get; set; }

        private bool Exit { get; set; } = false;
        public MainPage()
        {
            this.InitializeComponent();
            if (UseRNTester)
            {
                treeDump.Visibility = Visibility.Visible;
                myRootView.ComponentName = "RNTesterApp";
                Unloaded += (object sender, RoutedEventArgs e) => { Exit = true; };
                var dq = DispatcherQueue.GetForCurrentThread();
                var dqt = dq.CreateTimer();
                dqt.Interval = TimeSpan.FromMilliseconds(300);
                dqt.IsRepeating = false;
                dqt.Tick += UpdateTreeDump;
                dq.ShutdownStarting += (DispatcherQueue dq_, DispatcherQueueShutdownStartingEventArgs _) => {
                    Exit = true; dqt.Stop();
                };
                dqt.Start();
            }
            var app = Application.Current as App;
            myRootView.ReactNativeHost = app.Host;
        }

        private async void UpdateTreeDump(DispatcherQueueTimer sender, object args)
        {
            try
            {
                var pageHeader = TreeDumpLibrary.TreeDumpHelper.FindChildWithMatchingUIAID(this, "PageHeader") as TextBlock;
                if (pageHeader != null)
                {
                    var matches = await TreeDumpLibrary.TreeDumpControlViewManager.DoesTreeDumpMatchForRNTester(myRootView);
                }
                if (Exit)
                {
                    sender.Stop();
                    return;
                }
                sender.Start();
            } catch (Exception e)
            {

            }
        }
    }
}
