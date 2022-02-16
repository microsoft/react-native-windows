
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

REACT_STRUCT(PushNotificationManagerIOSSpec_Permissions)
struct PushNotificationManagerIOSSpec_Permissions {
    REACT_FIELD(alert)
    bool alert;
    REACT_FIELD(badge)
    bool badge;
    REACT_FIELD(sound)
    bool sound;
};

REACT_STRUCT(PushNotificationManagerIOSSpec_Notification)
struct PushNotificationManagerIOSSpec_Notification {
    REACT_FIELD(alertTitle)
    std::optional<std::string> alertTitle;
    REACT_FIELD(fireDate)
    std::optional<double> fireDate;
    REACT_FIELD(alertBody)
    std::optional<std::string> alertBody;
    REACT_FIELD(alertAction)
    std::optional<std::string> alertAction;
    REACT_FIELD(userInfo)
    std::optional<::React::JSValue> userInfo;
    REACT_FIELD(category)
    std::optional<std::string> category;
    REACT_FIELD(repeatInterval)
    std::optional<std::string> repeatInterval;
    REACT_FIELD(applicationIconBadgeNumber)
    std::optional<double> applicationIconBadgeNumber;
    REACT_FIELD(isSilent)
    std::optional<bool> isSilent;
    REACT_FIELD(soundName)
    std::optional<std::string> soundName;
};

REACT_STRUCT(PushNotificationManagerIOSSpec_requestPermissions_permission)
struct PushNotificationManagerIOSSpec_requestPermissions_permission {
    REACT_FIELD(alert)
    bool alert;
    REACT_FIELD(badge)
    bool badge;
    REACT_FIELD(sound)
    bool sound;
};

struct PushNotificationManagerIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, std::string) noexcept>{0, L"onFinishRemoteNotification"},
      Method<void(double) noexcept>{1, L"setApplicationIconBadgeNumber"},
      Method<void(Callback<double>) noexcept>{2, L"getApplicationIconBadgeNumber"},
      Method<void(PushNotificationManagerIOSSpec_requestPermissions_permission, Promise<::React::JSValue>) noexcept>{3, L"requestPermissions"},
      Method<void() noexcept>{4, L"abandonPermissions"},
      Method<void(Callback<PushNotificationManagerIOSSpec_Permissions>) noexcept>{5, L"checkPermissions"},
      Method<void(PushNotificationManagerIOSSpec_Notification) noexcept>{6, L"presentLocalNotification"},
      Method<void(PushNotificationManagerIOSSpec_Notification) noexcept>{7, L"scheduleLocalNotification"},
      Method<void() noexcept>{8, L"cancelAllLocalNotifications"},
      Method<void(::React::JSValue) noexcept>{9, L"cancelLocalNotifications"},
      Method<void(Promise<::React::JSValue>) noexcept>{10, L"getInitialNotification"},
      Method<void(Callback<PushNotificationManagerIOSSpec_Notification>) noexcept>{11, L"getScheduledLocalNotifications"},
      Method<void() noexcept>{12, L"removeAllDeliveredNotifications"},
      Method<void(std::vector<std::string>) noexcept>{13, L"removeDeliveredNotifications"},
      Method<void(Callback<std::vector<PushNotificationManagerIOSSpec_Notification>>) noexcept>{14, L"getDeliveredNotifications"},
      Method<void(Callback<double>) noexcept>{15, L"getAuthorizationStatus"},
      Method<void(std::string) noexcept>{16, L"addListener"},
      Method<void(double) noexcept>{17, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PushNotificationManagerIOSSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "onFinishRemoteNotification",
          "    REACT_METHOD(onFinishRemoteNotification) void onFinishRemoteNotification(std::string notificationId, std::string fetchResult) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(onFinishRemoteNotification) static void onFinishRemoteNotification(std::string notificationId, std::string fetchResult) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "setApplicationIconBadgeNumber",
          "    REACT_METHOD(setApplicationIconBadgeNumber) void setApplicationIconBadgeNumber(double num) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setApplicationIconBadgeNumber) static void setApplicationIconBadgeNumber(double num) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getApplicationIconBadgeNumber",
          "    REACT_METHOD(getApplicationIconBadgeNumber) void getApplicationIconBadgeNumber(std::function<void(double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getApplicationIconBadgeNumber) static void getApplicationIconBadgeNumber(std::function<void(double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "requestPermissions",
          "    REACT_METHOD(requestPermissions) void requestPermissions(PushNotificationManagerIOSSpec_requestPermissions_permission && permission, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(requestPermissions) static void requestPermissions(PushNotificationManagerIOSSpec_requestPermissions_permission && permission, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "abandonPermissions",
          "    REACT_METHOD(abandonPermissions) void abandonPermissions() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(abandonPermissions) static void abandonPermissions() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "checkPermissions",
          "    REACT_METHOD(checkPermissions) void checkPermissions(std::function<void(PushNotificationManagerIOSSpec_Permissions const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(checkPermissions) static void checkPermissions(std::function<void(PushNotificationManagerIOSSpec_Permissions const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "presentLocalNotification",
          "    REACT_METHOD(presentLocalNotification) void presentLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(presentLocalNotification) static void presentLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "scheduleLocalNotification",
          "    REACT_METHOD(scheduleLocalNotification) void scheduleLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(scheduleLocalNotification) static void scheduleLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "cancelAllLocalNotifications",
          "    REACT_METHOD(cancelAllLocalNotifications) void cancelAllLocalNotifications() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(cancelAllLocalNotifications) static void cancelAllLocalNotifications() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "cancelLocalNotifications",
          "    REACT_METHOD(cancelLocalNotifications) void cancelLocalNotifications(::React::JSValue && userInfo) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(cancelLocalNotifications) static void cancelLocalNotifications(::React::JSValue && userInfo) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "getInitialNotification",
          "    REACT_METHOD(getInitialNotification) void getInitialNotification(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getInitialNotification) static void getInitialNotification(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "getScheduledLocalNotifications",
          "    REACT_METHOD(getScheduledLocalNotifications) void getScheduledLocalNotifications(std::function<void(PushNotificationManagerIOSSpec_Notification const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getScheduledLocalNotifications) static void getScheduledLocalNotifications(std::function<void(PushNotificationManagerIOSSpec_Notification const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "removeAllDeliveredNotifications",
          "    REACT_METHOD(removeAllDeliveredNotifications) void removeAllDeliveredNotifications() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeAllDeliveredNotifications) static void removeAllDeliveredNotifications() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "removeDeliveredNotifications",
          "    REACT_METHOD(removeDeliveredNotifications) void removeDeliveredNotifications(std::vector<std::string> const & identifiers) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeDeliveredNotifications) static void removeDeliveredNotifications(std::vector<std::string> const & identifiers) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "getDeliveredNotifications",
          "    REACT_METHOD(getDeliveredNotifications) void getDeliveredNotifications(std::function<void(std::vector<PushNotificationManagerIOSSpec_Notification> const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getDeliveredNotifications) static void getDeliveredNotifications(std::function<void(std::vector<PushNotificationManagerIOSSpec_Notification> const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "getAuthorizationStatus",
          "    REACT_METHOD(getAuthorizationStatus) void getAuthorizationStatus(std::function<void(double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getAuthorizationStatus) static void getAuthorizationStatus(std::function<void(double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventType) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventType) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
