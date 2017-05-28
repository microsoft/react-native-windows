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

        /// <summary>
        /// Gets the current badge number for the app icon on the home screen
        /// </summary>
        /// <param name="callback"></param>
        public void getApplicationIconBadgeNumber(ICallback callback)
        {

        }

        /// <summary>
        /// Cancel local notifications.
        /// 
        /// Optionally restricts the set of canceled notifications to those
        /// notifications whose `userInfo` fields match the corresponding fields
        /// in the `userInfo` argument.
        /// </summary>
        /// <param name="userInfo"></param>
        [ReactMethod]
        public void cancelLocalNotifications(JObject userInfo)
        {

        }

        /// <summary>
        /// Gets the local notifications that are currently scheduled
        /// </summary>
        /// <param name="callback"></param>
        [ReactMethod]
        public void getScheduledLocalNotifications(ICallback callback)
        {

        }

        /// <summary>
        /// Requests notification permissions from iOS, prompting the user's
        /// dialog box. By default, it will request all notification permissions, but
        /// subset of these can be requested by passing a map of requested permissions.
        /// 
        /// The following permissions are supported:
        ///  - `alert`
        ///  - `badge`
        ///  - `sound`
        ///  
        /// If a map is provided to the method, only the permissions with truthy values will be requested.
        /// 
        /// This method returns a promise that will resolve when the user accepts,
        /// rejects, or if the permissions were previously rejected. The promise
        /// resolves to the current state of the permission.
        /// </summary>
        /// <param name="permissions"></param>
        /// <param name="promise"></param>
        [ReactMethod]
        public void requestPermissions(JObject permissions, IPromise promise)
        {

        }

        /// <summary>
        /// Unregister for all remote notifications received via Push Notification service.
        /// </summary>
        [ReactMethod]
        public void abandonPermissions()
        {

        }

        /// <summary>
        /// See what push permissions are currently enabled. `callback` will be invoked with a permissions object:
        /// 
        /// - `alert`: boolean
        /// - `badge`: boolean
        /// - `sound`: boolean
        /// </summary>
        /// <param name="callback"></param>
        [ReactMethod]
        public void checkPermissions(ICallback callback)
        {

        }

        /// <summary>
        /// This method returns a promise that resolves to either the notification
        /// object if the app was launched by a push notification, or `null` otherwise.
        /// </summary>
        /// <param name="promise"></param>
        [ReactMethod]
        public void getInitialNotification(IPromise promise)
        {

        }
    }
}
