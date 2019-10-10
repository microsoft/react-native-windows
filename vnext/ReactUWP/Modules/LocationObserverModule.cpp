// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/ValueUtils.h>
#include <winrt/Windows.Devices.Geolocation.h>
#include "LocationObserverModule.h"
#include "Unicode.h"

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#pragma warning(pop)

#include <cxxreact/MessageQueueThread.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react {
namespace uwp {

//
// LocationObserverModule::LocationObserver
//

class LocationObserverModule::LocationObserver {
 public:
  LocationObserver(
      LocationObserverModule *parent,
      const std::shared_ptr<facebook::react::MessageQueueThread>
          &defaultQueueThread)
      : m_parent(parent), m_queueThread(defaultQueueThread) {}

  void Disconnect() {
    m_parent = nullptr;
  }

  void getCurrentPosition(
      const folly::dynamic &options,
      Callback successCallback,
      Callback errorCallback);
  void startObserving(const folly::dynamic &options);
  void stopObserving();
  void requestAuthorization();

 private:
  void sendEvent(std::string &&eventName, folly::dynamic &&parameters);

  LocationObserverModule *m_parent;
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
  winrt::event_revoker<winrt::Windows::Devices::Geolocation::IGeolocator>
      m_positionChanged;
  winrt::event_revoker<winrt::Windows::Devices::Geolocation::IGeolocator>
      m_statusChanged;
};

class LocationObserverOptions {
 public:
  bool highAccuracy = false;
  double timeout = 10000;
  double maxAge = std::numeric_limits<double>::max();
  double distanceFilter = 100;

  LocationObserverOptions(const folly::dynamic &options) {
    if (!options.empty()) {
      for (auto &opt : options.items()) {
        if (opt.first == "timeout")
          timeout = opt.second.asDouble();
        else if (opt.first == "maximumAge")
          maxAge = opt.second.asDouble();
        else if (opt.first == "enableHighAccuracy")
          highAccuracy = opt.second.asBool();
        else if (opt.first == "distanceFilter")
          distanceFilter = opt.second.asDouble();
      }
    }
  }
};

static folly::dynamic ObjFromGeopos(
    winrt::Windows::Devices::Geolocation::Geoposition &geopos) {
  auto timestamp = winrt::Windows::Foundation::TimeSpan(
      geopos.Coordinate().Timestamp().time_since_epoch());

  folly::dynamic result = folly::dynamic::object(
      "coords",
      folly::dynamic::object(
          "latitude", geopos.Coordinate().Point().Position().Latitude)(
          "longitude", geopos.Coordinate().Point().Position().Longitude)(
          "altitude", geopos.Coordinate().Point().Position().Altitude)(
          "accuracy", geopos.Coordinate().Accuracy()))(
      "timestamp", timestamp.count());

  // Since we can't pass NaN via json, don't include optional bearing/speed for
  // now when stationary
  double heading = geopos.Coordinate().Heading().Value();
  double speed = geopos.Coordinate().Speed().Value();
  if (!std::isnan(heading))
    result["coords"]["bearing"] = heading;
  if (!std::isnan(speed))
    result["coords"]["speed"] = speed;

  return result;
}

winrt::Windows::Foundation::IAsyncOperation<
    winrt::Windows::Devices::Geolocation::Geoposition>
GetGeoposition(
    winrt::Windows::Devices::Geolocation::Geolocator &geoLocator,
    LocationObserverOptions &options) {
  auto location = co_await geoLocator.GetGeopositionAsync(
      TimeSpanFromMs(options.maxAge), TimeSpanFromMs(options.timeout));
  return location;
}

void LocationObserverModule::LocationObserver::getCurrentPosition(
    const folly::dynamic &optionsIn,
    Callback successCallback,
    Callback errorCallback) {
  LocationObserverOptions options(optionsIn);

  winrt::Windows::Devices::Geolocation::Geolocator geoLocator;
  geoLocator.DesiredAccuracy(
      options.highAccuracy
          ? winrt::Windows::Devices::Geolocation::PositionAccuracy::High
          : winrt::Windows::Devices::Geolocation::PositionAccuracy::Default);
  geoLocator.MovementThreshold(options.distanceFilter);

  winrt::Windows::Foundation::IAsyncOperation<
      winrt::Windows::Devices::Geolocation::Geoposition>
      result;
  try {
    result = GetGeoposition(geoLocator, options);
    result.get();
  } catch (winrt::hresult_error const &e) {
    errorCallback({folly::dynamic::object("code", 1)(
        "message",
        "Error in GetGeoposition:" +
            facebook::react::unicode::utf16ToUtf8(e.message()))});
    return;
  }

  winrt::Windows::Devices::Geolocation::Geoposition geopos =
      result.GetResults();
  folly::dynamic pos = ObjFromGeopos(geopos);

  successCallback({pos});
}

void LocationObserverModule::LocationObserver::startObserving(
    const folly::dynamic &optionsIn) {
  LocationObserverOptions options(optionsIn);

  winrt::Windows::Devices::Geolocation::Geolocator geoLocator;
  geoLocator.DesiredAccuracy(
      options.highAccuracy
          ? winrt::Windows::Devices::Geolocation::PositionAccuracy::High
          : winrt::Windows::Devices::Geolocation::PositionAccuracy::Default);
  geoLocator.MovementThreshold(options.distanceFilter);

  m_positionChanged = geoLocator.PositionChanged(
      winrt::auto_revoke, [this](auto &&, auto &&args) {
        winrt::Windows::Devices::Geolocation::Geoposition geopos =
            args.Position();
        folly::dynamic pos = ObjFromGeopos(geopos);

        sendEvent("geolocationDidChange", std::move(pos));
      });

  m_statusChanged = geoLocator.StatusChanged(
      winrt::auto_revoke, [this](auto &&, auto &&args) {
        winrt::Windows::Devices::Geolocation::PositionStatus status =
            args.Status();
        switch (status) {
          case winrt::Windows::Devices::Geolocation::PositionStatus::Disabled:
            sendEvent("geolocationError", "Geolocation access is disabled");
            break;
          case winrt::Windows::Devices::Geolocation::PositionStatus::
              NotAvailable:
            sendEvent("geolocationError", "Geolocation access is not enabled");
            break;
          case winrt::Windows::Devices::Geolocation::PositionStatus::NoData:
          case winrt::Windows::Devices::Geolocation::PositionStatus::
              Initializing:
            sendEvent(
                "geolocationError",
                "Geolocation access is temporarily not available");
            break;

          default:
            break;
        }
      });
}

void LocationObserverModule::LocationObserver::stopObserving() {
  m_positionChanged.revoke();
  m_statusChanged.revoke();
}

void LocationObserverModule::LocationObserver::requestAuthorization() {
  m_queueThread->runOnQueue([]() {
    winrt::Windows::Devices::Geolocation::Geolocator geoLocator;
    geoLocator.RequestAccessAsync();
  });
}

void LocationObserverModule::LocationObserver::sendEvent(
    std::string &&eventName,
    folly::dynamic &&parameters) {
  if (!m_parent)
    return;

  auto instance = m_parent->getInstance().lock();
  if (instance)
    instance->callJSFunction(
        "RCTDeviceEventEmitter",
        "emit",
        folly::dynamic::array(eventName, std::move(parameters)));
}

//
// LocationObserverModule
//
const char *LocationObserverModule::name = "LocationObserver";

LocationObserverModule::LocationObserverModule(
    const std::shared_ptr<facebook::react::MessageQueueThread>
        &defaultQueueThread)
    : m_locationObserver(
          std::make_shared<LocationObserver>(this, defaultQueueThread)) {}

LocationObserverModule::~LocationObserverModule() {}

std::string LocationObserverModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> LocationObserverModule::getConstants() {
  std::map<std::string, folly::dynamic> constants{{}};

  return constants;
}

auto LocationObserverModule::getMethods() -> std::vector<Method> {
  std::shared_ptr<LocationObserver> locationObserver(m_locationObserver);
  return {
      Method(
          "getCurrentPosition",
          [locationObserver](
              folly::dynamic args,
              Callback successCallback,
              Callback errorCallback) {
            locationObserver->getCurrentPosition(
                facebook::xplat::jsArgAsDynamic(args, 0),
                successCallback,
                errorCallback);
          },
          AsyncTag),
      Method(
          "startObserving",
          [locationObserver](folly::dynamic args) {
            locationObserver->startObserving(
                facebook::xplat::jsArgAsDynamic(args, 0));
          }),
      Method(
          "stopObserving",
          [locationObserver]() { locationObserver->stopObserving(); }),
      Method(
          "requestAuthorization",
          [locationObserver]() { locationObserver->requestAuthorization(); }),
  };
}

} // namespace uwp
} // namespace react
