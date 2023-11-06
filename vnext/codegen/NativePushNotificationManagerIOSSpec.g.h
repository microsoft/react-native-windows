
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

struct PushNotificationManagerIOSSpec_Permissions {
    bool alert;
    bool badge;
    bool sound;
};

struct PushNotificationManagerIOSSpec_Notification {
    std::optional<std::string> alertTitle;
    std::optional<double> fireDate;
    std::optional<std::string> alertBody;
    std::optional<std::string> alertAction;
    std::optional<::React::JSValue> userInfo;
    std::optional<std::string> category;
    std::optional<std::string> repeatInterval;
    std::optional<double> applicationIconBadgeNumber;
    std::optional<bool> isSilent;
    std::optional<std::string> soundName;
};

struct PushNotificationManagerIOSSpec_requestPermissions_permission {
    bool alert;
    bool badge;
    bool sound;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PushNotificationManagerIOSSpec_Permissions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"alert", &PushNotificationManagerIOSSpec_Permissions::alert},
        {L"badge", &PushNotificationManagerIOSSpec_Permissions::badge},
        {L"sound", &PushNotificationManagerIOSSpec_Permissions::sound},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PushNotificationManagerIOSSpec_Notification*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"alertTitle", &PushNotificationManagerIOSSpec_Notification::alertTitle},
        {L"fireDate", &PushNotificationManagerIOSSpec_Notification::fireDate},
        {L"alertBody", &PushNotificationManagerIOSSpec_Notification::alertBody},
        {L"alertAction", &PushNotificationManagerIOSSpec_Notification::alertAction},
        {L"userInfo", &PushNotificationManagerIOSSpec_Notification::userInfo},
        {L"category", &PushNotificationManagerIOSSpec_Notification::category},
        {L"repeatInterval", &PushNotificationManagerIOSSpec_Notification::repeatInterval},
        {L"applicationIconBadgeNumber", &PushNotificationManagerIOSSpec_Notification::applicationIconBadgeNumber},
        {L"isSilent", &PushNotificationManagerIOSSpec_Notification::isSilent},
        {L"soundName", &PushNotificationManagerIOSSpec_Notification::soundName},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PushNotificationManagerIOSSpec_requestPermissions_permission*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"alert", &PushNotificationManagerIOSSpec_requestPermissions_permission::alert},
        {L"badge", &PushNotificationManagerIOSSpec_requestPermissions_permission::badge},
        {L"sound", &PushNotificationManagerIOSSpec_requestPermissions_permission::sound},
    };
    return fieldMap;
}

struct PushNotificationManagerIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, std::string) noexcept>{0, L"onFinishRemoteNotification"},
      Method<void(double) noexcept>{1, L"setApplicationIconBadgeNumber"},
      Method<void(Callback<double>) noexcept>{2, L"getApplicationIconBadgeNumber"},
      Method<void(PushNotificationManagerIOSSpec_requestPermissions_permission, Promise<PushNotificationManagerIOSSpec_Permissions>) noexcept>{3, L"requestPermissions"},
      Method<void() noexcept>{4, L"abandonPermissions"},
      Method<void(Callback<PushNotificationManagerIOSSpec_Permissions>) noexcept>{5, L"checkPermissions"},
      Method<void(PushNotificationManagerIOSSpec_Notification) noexcept>{6, L"presentLocalNotification"},
      Method<void(PushNotificationManagerIOSSpec_Notification) noexcept>{7, L"scheduleLocalNotification"},
      Method<void() noexcept>{8, L"cancelAllLocalNotifications"},
      Method<void(::React::JSValue) noexcept>{9, L"cancelLocalNotifications"},
      Method<void(Promise<std::optional<PushNotificationManagerIOSSpec_Notification>>) noexcept>{10, L"getInitialNotification"},
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
          "    REACT_METHOD(onFinishRemoteNotification) void onFinishRemoteNotification(std::string notificationId, std::string fetchResult) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(onFinishRemoteNotification) static void onFinishRemoteNotification(std::string notificationId, std::string fetchResult) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "setApplicationIconBadgeNumber",
          "    REACT_METHOD(setApplicationIconBadgeNumber) void setApplicationIconBadgeNumber(double num) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setApplicationIconBadgeNumber) static void setApplicationIconBadgeNumber(double num) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getApplicationIconBadgeNumber",
          "    REACT_METHOD(getApplicationIconBadgeNumber) void getApplicationIconBadgeNumber(std::function<void(double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getApplicationIconBadgeNumber) static void getApplicationIconBadgeNumber(std::function<void(double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "requestPermissions",
          "    REACT_METHOD(requestPermissions) void requestPermissions(PushNotificationManagerIOSSpec_requestPermissions_permission && permission, ::React::ReactPromise<PushNotificationManagerIOSSpec_Permissions> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(requestPermissions) static void requestPermissions(PushNotificationManagerIOSSpec_requestPermissions_permission && permission, ::React::ReactPromise<PushNotificationManagerIOSSpec_Permissions> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "abandonPermissions",
          "    REACT_METHOD(abandonPermissions) void abandonPermissions() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(abandonPermissions) static void abandonPermissions() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "checkPermissions",
          "    REACT_METHOD(checkPermissions) void checkPermissions(std::function<void(PushNotificationManagerIOSSpec_Permissions const &)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(checkPermissions) static void checkPermissions(std::function<void(PushNotificationManagerIOSSpec_Permissions const &)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "presentLocalNotification",
          "    REACT_METHOD(presentLocalNotification) void presentLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(presentLocalNotification) static void presentLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "scheduleLocalNotification",
          "    REACT_METHOD(scheduleLocalNotification) void scheduleLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(scheduleLocalNotification) static void scheduleLocalNotification(PushNotificationManagerIOSSpec_Notification && notification) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "cancelAllLocalNotifications",
          "    REACT_METHOD(cancelAllLocalNotifications) void cancelAllLocalNotifications() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(cancelAllLocalNotifications) static void cancelAllLocalNotifications() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "cancelLocalNotifications",
          "    REACT_METHOD(cancelLocalNotifications) void cancelLocalNotifications(::React::JSValue && userInfo) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(cancelLocalNotifications) static void cancelLocalNotifications(::React::JSValue && userInfo) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "getInitialNotification",
          "    REACT_METHOD(getInitialNotification) void getInitialNotification(::React::ReactPromise<std::optional<PushNotificationManagerIOSSpec_Notification>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getInitialNotification) static void getInitialNotification(::React::ReactPromise<std::optional<PushNotificationManagerIOSSpec_Notification>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "getScheduledLocalNotifications",
          "    REACT_METHOD(getScheduledLocalNotifications) void getScheduledLocalNotifications(std::function<void(PushNotificationManagerIOSSpec_Notification const &)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getScheduledLocalNotifications) static void getScheduledLocalNotifications(std::function<void(PushNotificationManagerIOSSpec_Notification const &)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "removeAllDeliveredNotifications",
          "    REACT_METHOD(removeAllDeliveredNotifications) void removeAllDeliveredNotifications() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeAllDeliveredNotifications) static void removeAllDeliveredNotifications() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "removeDeliveredNotifications",
          "    REACT_METHOD(removeDeliveredNotifications) void removeDeliveredNotifications(std::vector<std::string> const & identifiers) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeDeliveredNotifications) static void removeDeliveredNotifications(std::vector<std::string> const & identifiers) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "getDeliveredNotifications",
          "    REACT_METHOD(getDeliveredNotifications) void getDeliveredNotifications(std::function<void(std::vector<PushNotificationManagerIOSSpec_Notification> const &)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getDeliveredNotifications) static void getDeliveredNotifications(std::function<void(std::vector<PushNotificationManagerIOSSpec_Notification> const &)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "getAuthorizationStatus",
          "    REACT_METHOD(getAuthorizationStatus) void getAuthorizationStatus(std::function<void(double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getAuthorizationStatus) static void getAuthorizationStatus(std::function<void(double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventType) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
