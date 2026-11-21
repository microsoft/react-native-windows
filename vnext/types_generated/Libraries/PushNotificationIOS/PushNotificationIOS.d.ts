/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<d322e7cccedecfee59b951045ab66b04>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\PushNotificationIOS\PushNotificationIOS.js
 */

export type PushNotificationPermissions = {
  alert: boolean;
  badge: boolean;
  sound: boolean;
  [key: string]: boolean | number;
};
type PresentLocalNotificationDetails = {
  alertBody: string;
  alertAction?: string | undefined;
  alertTitle?: string | undefined;
  soundName?: string | undefined;
  category?: string | undefined;
  userInfo?: Object | undefined;
  applicationIconBadgeNumber?: number | undefined;
  fireDate?: number | undefined;
  isSilent?: boolean | undefined;
};
type ScheduleLocalNotificationDetails = Omit<PresentLocalNotificationDetails, "repeatInterval"> & {
  repeatInterval?: "year" | "month" | "week" | "day" | "hour" | "minute" | undefined;
};
export type ContentAvailable = 1 | null | void;
export type FetchResult = {
  NewData: "UIBackgroundFetchResultNewData";
  NoData: "UIBackgroundFetchResultNoData";
  ResultFailed: "UIBackgroundFetchResultFailed";
};
/**
 * An event emitted by PushNotificationIOS.
 */
export type PushNotificationEventName = keyof {
  /**
   * Fired when a remote notification is received. The handler will be invoked
   * with an instance of `PushNotificationIOS`. This will handle notifications
   * that arrive in the foreground or were tapped to open the app from the
   * background.
   */
  notification: string;
  /**
   * Fired when a local notification is received. The handler will be invoked
   * with an instance of `PushNotificationIOS`. This will handle notifications
   * that arrive in the foreground or were tapped to open the app from the
   * background.
   */
  localNotification: string;
  /**
   * Fired when the user registers for remote notifications. The handler will be
   * invoked with a hex string representing the deviceToken.
   */
  register: string;
  /**
   * Fired when the user fails to register for remote notifications. Typically
   * occurs due to APNS issues or if the device is a simulator. The handler
   * will be invoked with {message: string, code: number, details: any}.
   */
  registrationError: string;
};
export interface PushNotification {
  /**
   * An alias for `getAlert` to get the notification's main message string
   */
  getMessage(): (string | undefined) | (Object | undefined);
  /**
   * Gets the sound string from the `aps` object
   */
  getSound(): string | undefined;
  /**
   * Gets the category string from the `aps` object
   */
  getCategory(): string | undefined;
  /**
   * Gets the notification's main message from the `aps` object
   */
  getAlert(): (string | undefined) | (Object | undefined);
  /**
   * Gets the content-available number from the `aps` object
   */
  getContentAvailable(): ContentAvailable;
  /**
   * Gets the badge count number from the `aps` object
   */
  getBadgeCount(): number | undefined;
  /**
   * Gets the data object on the notif
   */
  getData(): Object | undefined;
  /**
   * Gets the thread ID on the notif
   */
  getThreadID(): string | undefined;
  /**
   * iOS Only
   * Signifies remote notification handling is complete
   */
  finish(result: string): void;
}
/**
 * Handle notifications for your app, including scheduling and permissions.
 *
 * See https://reactnative.dev/docs/pushnotificationios
 *
 * @deprecated Use [@react-native-community/push-notification-ios](https://www.npmjs.com/package/@react-native-community/push-notification-ios) instead
 */
declare class PushNotificationIOS {
  /**
   * iOS fetch results that best describe the result of a finished remote notification handler.
   * For a list of possible values, see `PushNotificationIOS.FetchResult`.
   */
  static FetchResult: FetchResult;
  /**
   * Schedules the localNotification for immediate presentation.
   * details is an object containing:
   * alertBody : The message displayed in the notification alert.
   * alertAction : The "action" displayed beneath an actionable notification. Defaults to "view";
   * soundName : The sound played when the notification is fired (optional).
   * category : The category of this notification, required for actionable notifications (optional).
   * userInfo : An optional object containing additional notification data.
   * applicationIconBadgeNumber (optional) : The number to display as the app's icon badge. The default value of this property is 0, which means that no badge is displayed.
   *
   * See https://reactnative.dev/docs/pushnotificationios#presentlocalnotification
   */
  static presentLocalNotification(details: PresentLocalNotificationDetails): void;
  /**
   * Schedules a local notification for future presentation.
   *
   * details is an object containing:
   * fireDate : The date and time when the system should deliver the notification.
   * alertBody : The message displayed in the notification alert.
   * alertAction : The "action" displayed beneath an actionable notification. Defaults to "view";
   * soundName : The sound played when the notification is fired (optional).
   * category : The category of this notification, required for actionable notifications (optional).
   * userInfo : An optional object containing additional notification data.
   * applicationIconBadgeNumber (optional) : The number to display as the app's icon badge. Setting the number to 0 removes the icon badge.
   *
   * See https://reactnative.dev/docs/pushnotificationios#schedulelocalnotification
   */
  static scheduleLocalNotification(details: ScheduleLocalNotificationDetails): void;
  /**
   * Cancels all scheduled local notifications.
   *
   * See https://reactnative.dev/docs/pushnotificationios#cancelalllocalnotifications
   */
  static cancelAllLocalNotifications(): void;
  /**
   * Removes all delivered notifications from Notification Center.
   *
   * See https://reactnative.dev/docs/pushnotificationios#removealldeliverednotifications
   */
  static removeAllDeliveredNotifications(): void;
  /**
   * Provides a list of the app’s notifications that are currently displayed
   * in Notification Center.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getdeliverednotifications
   */
  static getDeliveredNotifications(callback: (notifications: Array<Object>) => void): void;
  /**
   * Removes the specified notifications from Notification Center.
   *
   * See https://reactnative.dev/docs/pushnotificationios#removedeliverednotifications
   */
  static removeDeliveredNotifications(identifiers: Array<string>): void;
  /**
   * Sets the badge number for the app icon on the Home Screen.
   *
   * See https://reactnative.dev/docs/pushnotificationios#setapplicationiconbadgenumber
   */
  static setApplicationIconBadgeNumber(number: number): void;
  /**
   * Gets the current badge number for the app icon on the Home Screen.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getapplicationiconbadgenumber
   */
  static getApplicationIconBadgeNumber(callback: Function): void;
  /**
   * Cancels any scheduled local notifications which match the fields in the
   * provided `userInfo`.
   *
   * See https://reactnative.dev/docs/pushnotificationios#cancellocalnotification
   */
  static cancelLocalNotifications(userInfo: Object): void;
  /**
   * Gets the list of local notifications that are currently scheduled.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getscheduledlocalnotifications
   */
  static getScheduledLocalNotifications(callback: Function): void;
  /**
   * Attaches a listener to notification events including local notifications,
   * remote notifications, and notification registration results.
   *
   * See https://reactnative.dev/docs/pushnotificationios#addeventlistener
   */
  static addEventListener(type: PushNotificationEventName, handler: Function): void;
  /**
   * Removes the event listener. Do this in `componentWillUnmount` to prevent
   * memory leaks.
   *
   * See https://reactnative.dev/docs/pushnotificationios#removeeventlistener
   */
  static removeEventListener(type: PushNotificationEventName): void;
  /**
   * Requests notification permissions from iOS, prompting the user with a
   * dialog box. By default, it will request all notification permissions, but
   * you can optionally specify which permissions to request.
   *
   * See https://reactnative.dev/docs/pushnotificationios#requestpermissions
   */
  static requestPermissions(permissions?: PushNotificationPermissions): Promise<{
    alert: boolean;
    badge: boolean;
    sound: boolean;
  }>;
  /**
   * Unregister for all remote notifications received via Apple Push Notification
   * service.
   * You should call this method in rare circumstances only, such as when
   * a new version of the app removes support for all types of remote
   * notifications. Users can temporarily prevent apps from receiving
   * remote notifications through the Notifications section of the
   * Settings app. Apps unregistered through this method can always
   * re-register.
   *
   * See https://reactnative.dev/docs/pushnotificationios#abandonpermissions
   */
  static abandonPermissions(): void;
  /**
   * Check which push permissions are currently enabled. `callback` will be
   * invoked with a `Permissions` object.
   *
   *  - `alert` :boolean
   *  - `badge` :boolean
   *  - `sound` :boolean
   *
   * See https://reactnative.dev/docs/pushnotificationios#checkpermissions
   */
  static checkPermissions(callback: (permissions: PushNotificationPermissions) => void): void;
  /**
   * This method returns a promise that resolves to either the notification
   * object if the app was launched by a push notification, or `null` otherwise.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getinitialnotification
   */
  static getInitialNotification(): Promise<null | undefined | PushNotification>;
  /**
   * This method returns a promise that resolves to the current notification
   * authorization status. See UNAuthorizationStatus for possible values.
   */
  static getAuthorizationStatus(callback: (authorizationStatus: number) => void): void;
  /**
   * You will never need to instantiate `PushNotificationIOS` yourself.
   * Listening to the `notification` event and invoking
   * `getInitialNotification` is sufficient.
   *
   */
  constructor(nativeNotif: Object);
  /**
   * This method is available for remote notifications that have been received via:
   * `application:didReceiveRemoteNotification:fetchCompletionHandler:`. See docs
   * for more information.
   *
   * See https://reactnative.dev/docs/pushnotificationios#finish
   */
  finish(fetchResult: string): void;
  /**
   * An alias for `getAlert` to get the notification's main message string.
   */
  getMessage(): (null | undefined | string) | (null | undefined | Object);
  /**
   * Gets the sound string from the `aps` object. This will be `null` for local
   * notifications.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getsound
   */
  getSound(): null | undefined | string;
  /**
   * Gets the category string from the `aps` object.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getcategory
   */
  getCategory(): null | undefined | string;
  /**
   * Gets the notification's main message from the `aps` object. Also see the
   * alias: `getMessage()`.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getalert
   */
  getAlert(): (null | undefined | string) | (null | undefined | Object);
  /**
   * Gets the content-available number from the `aps` object.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getcontentavailable
   */
  getContentAvailable(): ContentAvailable;
  /**
   * Gets the badge count number from the `aps` object.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getbadgecount
   */
  getBadgeCount(): null | undefined | number;
  /**
   * Gets the data object on the notification.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getdata
   */
  getData(): null | undefined | Object;
  /**
   * Gets the thread ID on the notification.
   *
   * See https://reactnative.dev/docs/pushnotificationios#getthreadid
   */
  getThreadID(): null | undefined | string;
}
export default PushNotificationIOS;
