using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Networking.PushNotifications;
using Windows.UI.Notifications;

namespace ReactNative.Modules.PushNotification
{
    /// <summary>
    /// A module that allows JS to access push notifications.
    /// </summary>
    class PushNotificationModule : ReactContextNativeModuleBase
    {
        private PushNotificationChannel _channel = null;
        private uint _badgeNumber = 0;

        /// <summary>
        /// Instantiates the <see cref="PushNotificationModule"/>.
        /// </summary>
        internal PushNotificationModule(ReactContext reactContext)
            : base(reactContext)
        {
        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "PushNotificationManager";
            }
        }

        /// <summary>
        /// Initialize application for remote push notifications.
        /// </summary>
        [ReactMethod]
        public void requestPermissions(JObject o)
        {
            Task.Run(async () =>
            {
                try
                {
                     _channel = await PushNotificationChannelManager.CreatePushNotificationChannelForApplicationAsync();
                }
                catch (Exception)
                {
                    // TODO: Handle exceptions
                }

                if (_channel != null)
                {                 
                    _channel.PushNotificationReceived += OnPushNotificationReceived;
                    EventEmitter.emit("remoteNotificationsRegistered", new JObject
                    {
                        { "deviceToken", _channel.Uri },
                    });
                }
            });
        }

        /// <summary>
        /// Abandon remote push notifications.
        /// </summary>
        [ReactMethod]
        public void abandonPermissions()
        { 
            _channel.PushNotificationReceived -= OnPushNotificationReceived;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="callback"></param>
        [ReactMethod]
        public void checkPermissions(ICallback callback)
        {
            callback.Invoke(new JObject
            {
                {"alert", true},
                {"badge", true},
                {"sound", false},
            });
        }

        /// <summary>
        /// Show local toast.
        /// </summary>
        /// <param name="details">Toast details.</param>
        [ReactMethod]
        public void presentLocalNotification(JObject details)
        {
            JToken body;
            if (details.TryGetValue("alertBody", out body) && body.Type == JTokenType.String)
            {
                var toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);
                var toastTextElements = toastXml.GetElementsByTagName("text");

                toastTextElements[0].AppendChild(toastXml.CreateTextNode(body.Value<string>()));

                ShowToast(toastXml);
            }          
        }

        /// <summary>
        /// Schedule local toast.
        /// </summary>
        /// <param name="details">Toast details.</param>
        [ReactMethod]
        public void scheduleLocalNotification(JObject details)
        {
            JToken body;
            if (details.TryGetValue("alertBody", out body) && body.Type == JTokenType.String)
            {
                var toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);
                var toastTextElements = toastXml.GetElementsByTagName("text");

                toastTextElements[0].AppendChild(toastXml.CreateTextNode(body.Value<string>()));

                JToken fireDate;
                if (details.TryGetValue("fireDate", out fireDate) && fireDate.Type == JTokenType.Date)
                {
                    var toast = new ScheduledToastNotification(toastXml, fireDate.Value<DateTime>());

                    ToastNotificationManager.CreateToastNotifier().AddToSchedule(toast);
                }                
            }
        }

        /// <summary>
        /// Cancel all scheduled toasts.
        /// </summary>
        [ReactMethod]
        public void cancelAllLocalNotifications()
        {
            var notifications = ToastNotificationManager.CreateToastNotifier().GetScheduledToastNotifications();
            foreach (var notification in notifications)
            {
                ToastNotificationManager.CreateToastNotifier().RemoveFromSchedule(notification);
            }
        }

        /// <summary>
        /// Set application badge number.
        /// </summary>
        /// <param name="number"></param>
        [ReactMethod]
        public void setApplicationIconBadgeNumber(uint number)
        {
            _badgeNumber = number;

            var badgeXml = BadgeUpdateManager.GetTemplateContent(BadgeTemplateType.BadgeNumber);
            var badgeElement = badgeXml.SelectSingleNode("/badge") as XmlElement;

            badgeElement.SetAttribute("value", number.ToString());
            UpdateBadge(badgeXml);
        }

        /// <summary>
        /// Get application bagde number.
        /// </summary>
        /// <param name="callback">The callback.</param>
        [ReactMethod]
        public void getApplicationIconBadgeNumber(ICallback callback)
        {
            callback.Invoke(_badgeNumber);
        }

        void OnPushNotificationReceived(PushNotificationChannel sender, PushNotificationReceivedEventArgs e)
        {
            var typeString = string.Empty;
            string notificationContent = String.Empty;
            switch (e.NotificationType)
            {
                case PushNotificationType.Badge:
                    typeString = "Badge";
                    var badgeXml = e.BadgeNotification.Content;
                    notificationContent = badgeXml.GetXml();

                    var badgeElement = badgeXml.SelectSingleNode("/badge") as XmlElement;
                    _badgeNumber = uint.Parse(badgeElement.GetAttribute("value"));

                    UpdateBadge(badgeXml);

                    EventEmitter.emit("remoteNotificationReceived", new JObject
                    {
                        { "_badgeCount",  _badgeNumber },
                    });

                    break;

                case PushNotificationType.Tile:
                    notificationContent = e.TileNotification.Content.GetXml();
                    typeString = "Tile";
                    break;

                case PushNotificationType.Toast:
                    notificationContent = e.ToastNotification.Content.GetXml();
                    typeString = "Toast";
                    // Setting the cancel property prevents the notification from being delivered. It's especially important to do this for toasts:
                    // if your application is already on the screen, there's no need to display a toast from push notifications.
                    e.Cancel = true;

                    EventEmitter.emit("remoteNotificationReceived", new JObject
                    {
                        { "_alert",  notificationContent },
                    });

                    break;

                case PushNotificationType.Raw:
                    notificationContent = e.RawNotification.Content;
                    typeString = "Raw";

                    EventEmitter.emit("remoteNotificationReceived", new JObject
                    {
                        { "_data", notificationContent },
                    });

                    break;
            }
        }

        private void UpdateBadge(XmlDocument badgeXml)
        {
            var badge = new BadgeNotification(badgeXml);
            var badgeUpdater = BadgeUpdateManager.CreateBadgeUpdaterForApplication();

            badgeUpdater.Update(badge);
        }

        private void ShowToast(XmlDocument toastXml)
        {
            var toast = new ToastNotification(toastXml);
            ToastNotificationManager.CreateToastNotifier().Show(toast);
        }
  
        private RCTDeviceEventEmitter EventEmitter
        {
            get
            {
                return Context.GetJavaScriptModule<RCTDeviceEventEmitter>();
            }
        }
    }
}
