using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using Windows.Networking.PushNotifications;

namespace ReactNative.Modules.PushNotification
{
    /// <summary>
    /// This module represents native push notifications.
    /// </summary>
    class PushNotificationModule : ReactContextNativeModuleBase
    {
        private const string ModuleName = "PushNotificationWindows";
        private int _badgeNumber;
        private PushNotificationChannel _channel;

        internal PushNotificationModule(ReactContext reactContext) : base(reactContext)
        {
        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return ModuleName;
            }
        }

        /// <summary>
        /// Schedules the localNotification for immediate presentation.
        /// </summary>
        /// <param name="details">
        /// details is an object containing:
        /// - `alertBody` : The message displayed in the notification alert.
        /// - `alertAction` : The "action" displayed beneath an actionable notification. Defaults to "view";
        /// - `soundName` : The sound played when the notification is fired (optional).
        /// - `category`  : The category of this notification, required for actionable notifications (optional).
        /// - `userInfo`  : An optional object containing additional notification data.
        /// - `applicationIconBadgeNumber` (optional) : The number to display as the app's icon badge. The default value of this property is 0, which means that no badge is displayed.
        /// </param>
        [ReactMethod]
        public void presentLocalNotification(JObject details)
        {

        }

        /// <summary>
        /// Schedules the localNotification for future presentation.
        /// </summary>
        /// <param name="details">
        /// details is an object containing:
        /// - `fireDate` : The date and time when the system should deliver the notification.
        /// - `alertBody` : The message displayed in the notification alert.
        /// - `alertAction` : The "action" displayed beneath an actionable notification. Defaults to "view";
        /// - `soundName` : The sound played when the notification is fired (optional).
        /// - `category`  : The category of this notification, required for actionable notifications (optional).
        /// - `userInfo` : An optional object containing additional notification data.
        /// - `applicationIconBadgeNumber` (optional) : The number to display as the app's icon badge. Setting the number to 0 removes the icon badge.
        /// /param>
        [ReactMethod]
        public void scheduleLocalNotification(JObject details)
        {

        }

        /// <summary>
        /// Cancels all scheduled localNotifications
        /// </summary>
        [ReactMethod]
        public void cancelAllLocalNotifications()
        {

        }

        /// <summary>
        /// Sets the badge number for the app icon on the home screen
        /// </summary>
        /// <param name="number">The badge number for the app icon on the home screen</param>
        [ReactMethod]
        public void setApplicationIconBadgeNumber(int number)
        {

        }

        [ReactMethod]
        public void getScheduledLocalNotifications(ICallback callback)
        {

        }

        [ReactMethod]
        public void cancelLocalNotifications(JObject userInfo)
        {

        }

        [ReactMethod]
        public void requestPermissions(JObject permissions, IPromise promise)
        {

        }

        [ReactMethod]
        public void abandonPermissions()
        {

        }

        [ReactMethod]
        public void checkPermissions(ICallback callback)
        {

        }

        [ReactMethod]
        public void getInitialNotification(IPromise promise)
        {

        }
    }
}
