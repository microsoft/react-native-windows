using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using ReactNative.Modules.Core;
using System;
using System.Linq;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Threading.Tasks;
using Windows.Devices.Geolocation;
using Windows.Foundation;

namespace ReactNative.Modules.Location
{
    class LocationModule : ReactContextNativeModuleBase
    {
        private readonly SerialDisposable _currentSubscription = new SerialDisposable();

        public LocationModule(ReactContext reactContext) 
            : base(reactContext)
        {
        }

        public override string Name
        {
            get
            {
                return "LocationObserver";
            }
        }

        [ReactMethod]
        public async void getCurrentPosition(
            JObject options,
            ICallback success,
            ICallback error)
        {
            var locationOptions = LocationOptions.FromJson(options);

            var geolocator = new Geolocator
            {
                DesiredAccuracy = locationOptions.HighAccuracy ? PositionAccuracy.High : PositionAccuracy.Default,
                MovementThreshold = locationOptions.DistanceFilter,
            };

            try
            {
                // TODO: Enable retrieval from position history using `MaximumAge` filter

                var task = geolocator.GetGeopositionAsync().AsTask();
                var completedTask = await Task.WhenAny(
                    task, 
                    Task.Delay(TimeSpan.FromMilliseconds(locationOptions.Timeout))).ConfigureAwait(false);

                if (completedTask == task)
                {
                    var geoposition = await task.ConfigureAwait(false);
                    success.Invoke(ConvertGeoposition(geoposition));
                }
                else
                {
                    error.Invoke("Location request timed out");
                }
            }
            catch (Exception ex)
            {
                error.Invoke($"Location request failed with exception: {ex}");
            }
        }

        [ReactMethod]
        public void startObserving(JObject options)
        {
            var locationOptions = LocationOptions.FromJson(options);
            var geolocator = new Geolocator
            {
                DesiredAccuracy = locationOptions.HighAccuracy ? PositionAccuracy.High : PositionAccuracy.Default,
                MovementThreshold = locationOptions.DistanceFilter,
            };

            var positionObservable = 
                Observable.FromEventPattern<
                    TypedEventHandler<Geolocator, PositionChangedEventArgs>,
                    Geolocator, 
                    PositionChangedEventArgs>(
                    h => geolocator.PositionChanged += h,
                    h => geolocator.PositionChanged -= h);

            var positionQuery = from e in positionObservable
                                select e.EventArgs.Position;

            var positionSubscription = positionQuery.Subscribe(position =>
                Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                    .emit("geolocationDidChange", ConvertGeoposition(position)));

            var statusObservable =
                Observable.FromEventPattern<
                    TypedEventHandler<Geolocator, StatusChangedEventArgs>,
                    Geolocator,
                    StatusChangedEventArgs>(
                        h => geolocator.StatusChanged += h,
                        h => geolocator.StatusChanged -= h);

            var statusQuery = from e in statusObservable
                              select e.EventArgs.Status;

            var statusSubscription = statusQuery.Subscribe(status =>
            {
                if (status == PositionStatus.Disabled)
                {
                    Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                        .emit("geolocationError", "Location settings are turned off.");
                }
                else if (status == PositionStatus.NotAvailable)
                {
                    Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                        .emit("geolocationError", "Location services is not available.");
                }
                else if (status == PositionStatus.NoData || status == PositionStatus.Initializing)
                {
                    Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                        .emit("geolocationError", "Location services is temporarily unavailable.");
                }
            });

            _currentSubscription.Disposable =
                new CompositeDisposable(2)
                {
                    positionSubscription,
                    statusSubscription,
                };
        }

        [ReactMethod]
        public void stopObserving()
        {
            _currentSubscription.Disposable = Disposable.Empty;
        }

        private static JObject ConvertGeoposition(Geoposition geoposition)
        {
            var coords = new JObject
            {
                { "latitude", geoposition.Coordinate.Point.Position.Latitude },
                { "longitude", geoposition.Coordinate.Point.Position.Longitude },
                { "altitude", geoposition.Coordinate.Point.Position.Altitude },
                { "accuracy", geoposition.Coordinate.Accuracy },
                { "bearing", geoposition.Coordinate.Heading },
                { "speed", geoposition.Coordinate.Speed }
            };

            return new JObject
            {
                { "coords", coords },
                { "timestamp", geoposition.Coordinate.Timestamp.ToUnixTimeMilliseconds() },
            };
        }

        class LocationOptions
        {
            private const double DefaultLocationAccuracy = 100;
            private const double DefaultMaximumAge = 86.4e6 /* TimeSpan.FromDays(1).TotalMilliseconds */;

            private LocationOptions(long timeout, double maximumAge, bool highAccuracy, double distanceFilter)
            {
                Timeout = timeout;
                MaximumAge = maximumAge;
                HighAccuracy = highAccuracy;
                DistanceFilter = distanceFilter;
            }

            public long Timeout { get; }

            public double MaximumAge { get; }

            public bool HighAccuracy { get; }

            public double DistanceFilter { get; }

            public static LocationOptions FromJson(JObject json)
            {
                var timeout = 
                    json.ContainsKey("timeout") ? json.Value<long>("timeout") : -1;
                var maximumAge = 
                    json.ContainsKey("maximumAge") ? json.Value<double>("maximumAge") : double.PositiveInfinity;
                var highAccuracy =
                    json.ContainsKey("enableHighAccuracy") && json.Value<bool>("enableHighAccuracy");
                var distanceFilter =
                    json.ContainsKey("distanceFilter") ? json.Value<double>("distanceFilter") : 100;

                return new LocationOptions(timeout, maximumAge, highAccuracy, distanceFilter);
            }
        }
    }
}
