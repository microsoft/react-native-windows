using Newtonsoft.Json.Linq;
using ReactNative.Collections;
using ReactNative.Modules.Image;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The view manager responsible for rendering native images.
    /// </summary>
    public class ReactImageManager : SimpleViewManager<Border>
    {
        private readonly Dictionary<int, SerialDisposable> _disposables =
            new Dictionary<int, SerialDisposable>();

        private readonly IImageCache _imageCache;

        private readonly Dictionary<int, List<KeyValuePair<string, double>>> _imageSources =
            new Dictionary<int, List<KeyValuePair<string, double>>>();

        /// <summary>
        /// Instantiates the <see cref="ReactImageManager"/>.  
        /// </summary>
        /// <param name="imageCache">The image cache.</param>
        public ReactImageManager(IImageCache imageCache)
        {
            _imageCache = imageCache;
        }

        /// <summary>
        /// The view manager name.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTImageView";
            }
        }

        /// <summary>
        /// The view manager event constants.
        /// </summary>
        public override IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    {
                        "topLoadStart",
                        new Dictionary<string, object>
                        {
                            { "registrationName", "onLoadStart" }
                        }
                    },
                    {
                        "topLoad",
                        new Dictionary<string, object>
                        {
                            { "registrationName", "onLoad" }
                        }
                    },
                    {
                        "topLoadEnd",
                        new Dictionary<string, object>
                        {
                            { "registrationName", "onLoadEnd" }
                        }
                    },
                };
            }
        }

        /// <summary>
        /// Set the scaling mode of the image.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="resizeMode">The scaling mode.</param>
        [ReactProp("resizeMode")]
        public void SetResizeMode(Border view, string resizeMode)
        {
            if (resizeMode !=  null)
            {
                var imageBrush = (ImageBrush)view.Background;

                if (resizeMode.Equals("cover"))
                {
                    imageBrush.Stretch = Stretch.UniformToFill;
                }
                else if (resizeMode.Equals("contain"))
                {
                    imageBrush.Stretch = Stretch.Uniform;
                }
                else
                {
                    imageBrush.Stretch = Stretch.Fill;
                }
            }
        }

        /// <summary>
        /// Set the source URI of the image.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="sources">The source URI.</param>
        [ReactProp("src")]
        public void SetSource(Border view, JArray sources)
        {
            var count = sources.Count;

            // There is no image source
            if (count == 0)
            {
                throw new ArgumentException("Sources must not be empty.", nameof(sources));
            }
            // Optimize for the case where we have just one uri, case in which we don't need the sizes
            else if (count == 1)
            {
                var uri = ((JObject)sources[0]).Value<string>("uri");
                SetUriFromSingleSource(view, uri);
            }
            else
            {
                var viewSources = default(List<KeyValuePair<string, double>>);
                var tag = view.GetTag();

                if (_imageSources.TryGetValue(tag, out viewSources))
                {
                    viewSources.Clear();
                }
                else
                {
                    viewSources = new List<KeyValuePair<string, double>>(count);
                    _imageSources.Add(tag, viewSources);
                }

                foreach (var source in sources)
                {
                    var sourceData = (JObject)source;
                    viewSources.Add(
                        new KeyValuePair<string, double>(
                            sourceData.Value<string>("uri"),
                            sourceData.Value<double>("width") * sourceData.Value<double>("height")));
                }

                viewSources.Sort((p1, p2) => p1.Value.CompareTo(p2.Value));

                if (double.IsNaN(view.Width) || double.IsNaN(view.Height))
                {
                    // If we need to choose from multiple URIs but the size is not yet set, wait for layout pass
                    return;
                }

                SetUriFromMultipleSources(view);
            }
        }
        
        /// <summary>
        /// The border radius of the <see cref="ReactRootView"/>.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="radius">The border radius value.</param>
        [ReactProp("borderRadius")]
        public void SetBorderRadius(Border view, double radius)
        {
            view.CornerRadius = new CornerRadius(radius);
        }

        /// <summary>
        /// Set the border color of the image view.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp("borderColor", CustomType = "Color")]
        public void SetBorderColor(Border view, uint? color)
        {
            view.BorderBrush = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Sets the border thickness of the image view.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="index">The property index.</param>
        /// <param name="width">The border width in pixels.</param>
        [ReactPropGroup(
            ViewProps.BorderWidth,
            ViewProps.BorderLeftWidth,
            ViewProps.BorderRightWidth,
            ViewProps.BorderTopWidth,
            ViewProps.BorderBottomWidth,
            DefaultDouble = double.NaN)]
        public void SetBorderWidth(Border view, int index, double width)
        {
            view.SetBorderWidth(ViewProps.BorderSpacingTypes[index], width);
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, Border view)
        {
            var tag = view.GetTag();
            var disposable = default(SerialDisposable);
            if (_disposables.TryGetValue(tag, out disposable))
            {
                disposable.Dispose();
                _disposables.Remove(tag);
            }

            _imageSources.Remove(tag);
        }

        /// <summary>
        /// Creates the image view instance.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The image view instance.</returns>
        protected override Border CreateViewInstance(ThemedReactContext reactContext)
        {
            return new Border
            {
                Background = new ImageBrush
                {
                    Stretch = Stretch.UniformToFill,
                },
            };
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The output buffer.</param>
        public override void SetDimensions(Border view, Dimensions dimensions)
        {
            base.SetDimensions(view, dimensions);
            SetUriFromMultipleSources(view);
        }

        private void OnImageFailed(Border view)
        {
            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactImageLoadEvent(
                        view.GetTag(),
                        ReactImageLoadEvent.OnLoadEnd));
        }

        private void OnImageOpened(Border view)
        {
            var eventDispatcher = view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher;

            eventDispatcher.DispatchEvent(
                new ReactImageLoadEvent(
                    view.GetTag(),
                    ReactImageLoadEvent.OnLoad));

            eventDispatcher.DispatchEvent(
                new ReactImageLoadEvent(
                    view.GetTag(),
                    ReactImageLoadEvent.OnLoadEnd));
        }

        /// <summary>
        /// Set the source URI of the image.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="source">The source URI.</param>
        private void SetUriFromSingleSource(Border view, string source)
        {
            var imageBrush = (ImageBrush)view.Background;
            var tag = view.GetTag();

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactImageLoadEvent(
                        tag,
                        ReactImageLoadEvent.OnLoadStart));

            var reference = _imageCache.Get(source);
            var subscription = reference.LoadedObservable.Subscribe(
                _ =>
                {
                    imageBrush.ImageSource = reference.Image;
                    OnImageOpened(view);
                },
                _ => OnImageFailed(view));

            var disposable = default(SerialDisposable);
            if (!_disposables.TryGetValue(tag, out disposable))
            {
                disposable = new SerialDisposable();
                _disposables.Add(tag, disposable);
            }

            disposable.Disposable = new CompositeDisposable
            {
                reference,
                subscription,
            };
        }

        /// <summary>
        /// Chooses the uri with the size closest to the target image size. Must be called only after the
        /// layout pass when the sizes of the target image have been computed, and when there are at least
        /// two sources to choose from.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        private void SetUriFromMultipleSources(Border view)
        {
            var sources = default(List<KeyValuePair<string, double>>);
            if (_imageSources.TryGetValue(view.GetTag(), out sources))
            {
                var targetImageSize = view.Width * view.Height;
                var bestResult = sources.LocalMin((s) => Math.Abs(s.Value - targetImageSize));
                SetUriFromSingleSource(view, bestResult.Key);
            }
        }
    }
}
