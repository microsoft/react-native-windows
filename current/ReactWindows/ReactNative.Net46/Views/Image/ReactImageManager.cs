// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Collections;
using ReactNative.Modules.I18N;
using ReactNative.Modules.Image;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using System.Reactive.Disposables;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The view manager responsible for rendering native images.
    /// </summary>
    public class ReactImageManager : SimpleViewManager<Border>
    {
        private readonly Dictionary<int, SerialDisposable> _disposables =
            new Dictionary<int, SerialDisposable>();

        private readonly Dictionary<int, List<KeyValuePair<string, double>>> _imageSources =
            new Dictionary<int, List<KeyValuePair<string, double>>>();

        private readonly ViewKeyedDictionary<Border, CornerRadiusManager> _borderToRadii =
            new ViewKeyedDictionary<Border, CornerRadiusManager>();

        private readonly ViewKeyedDictionary<Border, ThicknessManager> _borderToThickness =
            new ViewKeyedDictionary<Border, ThicknessManager>();

        private readonly Lazy<ScaleTransform> _rtlScaleTransform = new Lazy<ScaleTransform>(() => new ScaleTransform
        {
            CenterX = 0.5,
            ScaleX = -1
        });

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
        public override JObject CustomDirectEventTypeConstants
        {
            get
            {
                return new JObject
                {
                    {
                        "topLoadStart",
                        new JObject
                        {
                            { "registrationName", "onLoadStart" }
                        }
                    },
                    {
                        "topLoad",
                        new JObject
                        {
                            { "registrationName", "onLoad" }
                        }
                    },
                    {
                        "topLoadEnd",
                        new JObject
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
        [ReactProp(ViewProps.ResizeMode)]
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
        /// Enum values correspond to positions of prop names in ReactPropGroup attribute
        /// applied to <see cref="SetBorderRadius(Border, int, double?)"/>
        /// </summary>
        private enum Radius
        {
            All,
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
        }

        /// <summary>
        /// The border radius of the <see cref="ReactRootView"/>.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="index">The prop index.</param>
        /// <param name="radius">The border radius value.</param>
        [ReactPropGroup(
            ViewProps.BorderRadius,
            ViewProps.BorderTopLeftRadius,
            ViewProps.BorderTopRightRadius,
            ViewProps.BorderBottomLeftRadius,
            ViewProps.BorderBottomRightRadius)]
        public void SetBorderRadius(Border view, int index, double? radius)
        {
            if (!_borderToRadii.TryGetValue(view, out var cornerRadiusManager))
            {
                cornerRadiusManager = new CornerRadiusManager();
                _borderToRadii.AddOrUpdate(view, cornerRadiusManager);
            }

            switch ((Radius)index)
            {
                case Radius.All:
                    cornerRadiusManager.Set(CornerRadiusManager.All, radius);
                    break;
                case Radius.TopLeft:
                    cornerRadiusManager.Set(CornerRadiusManager.TopLeft, radius);
                    break;
                case Radius.TopRight:
                    cornerRadiusManager.Set(CornerRadiusManager.TopRight, radius);
                    break;
                case Radius.BottomLeft:
                    cornerRadiusManager.Set(CornerRadiusManager.BottomLeft, radius);
                    break;
                case Radius.BottomRight:
                    cornerRadiusManager.Set(CornerRadiusManager.BottomRight, radius);
                    break;
            }
            view.CornerRadius = cornerRadiusManager.AsCornerRadius();
        }

        /// <summary>
        /// Set the border color of the image view.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.BorderColor, CustomType = "Color")]
        public void SetBorderColor(Border view, uint? color)
        {
            view.BorderBrush = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        /// <summary>
        /// Enum values correspond to positions of prop names in ReactPropGroup attribute
        /// applied to <see cref="SetBorderWidth(Border, int, double?)"/>
        /// </summary>
        private enum Width
        {
            All,
            Left,
            Right,
            Top,
            Bottom,
        }

        /// <summary>
        /// Sets the border thickness of the image view.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="index">The prop index.</param>
        /// <param name="width">The border width in pixels.</param>
        [ReactPropGroup(
            ViewProps.BorderWidth,
            ViewProps.BorderLeftWidth,
            ViewProps.BorderRightWidth,
            ViewProps.BorderTopWidth,
            ViewProps.BorderBottomWidth)]
        public void SetBorderWidth(Border view, int index, double? width)
        {
            if (!_borderToThickness.TryGetValue(view, out var thicknessManager))
            {
                thicknessManager = new ThicknessManager();
                _borderToThickness.AddOrUpdate(view, thicknessManager);
            }

            switch ((Width)index)
            {
                case Width.All:
                    thicknessManager.Set(ThicknessManager.All, width);
                    break;
                case Width.Left:
                    thicknessManager.Set(ThicknessManager.Left, width);
                    break;
                case Width.Right:
                    thicknessManager.Set(ThicknessManager.Right, width);
                    break;
                case Width.Top:
                    thicknessManager.Set(ThicknessManager.Top, width);
                    break;
                case Width.Bottom:
                    thicknessManager.Set(ThicknessManager.Bottom, width);
                    break;
            }
            view.BorderThickness = thicknessManager.AsThickness();
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for
        /// additional cleanup.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        public override void OnDropViewInstance(ThemedReactContext reactContext, Border view)
        {
            base.OnDropViewInstance(reactContext, view);

            var tag = view.GetTag();
            var disposable = default(SerialDisposable);
            if (_disposables.TryGetValue(tag, out disposable))
            {
                disposable.Dispose();
                _disposables.Remove(tag);
            }

            _imageSources.Remove(tag);
            _borderToRadii.Remove(view);
            _borderToThickness.Remove(view);
        }

        /// <summary>
        /// Creates the image view instance.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The image view instance.</returns>
        protected override Border CreateViewInstance(ThemedReactContext reactContext)
        {
            var border = new Border
            {
                Background = new ImageBrush
                {
                    Stretch = Stretch.UniformToFill,
                },
            };

            // Using a Border instead of a native Image has its advantages (round corner support, etc.), but
            // we have to take into account the automatic flipping that happens in RTL mode. We use a transform
            // to negate that flipping.
            // NOTE: This WPF implementation doesn't support updating this on the fly
            if (I18NUtil.IsRightToLeft)
            {
                border.Background.RelativeTransform = _rtlScaleTransform.Value;
            }

            return border;
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
            if (!view.HasTag())
            {
                // View may have been unmounted, ignore.
                return;
            }

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactImageLoadEvent(
                        view.GetTag(),
                        ReactImageLoadEvent.OnLoadEnd));
        }

        private void OnImageStatusUpdate(Border view, ImageStatusEventData status)
        {
            if (!view.HasTag())
            {
                // View may have been unmounted, ignore.
                return;
            }

            var eventDispatcher = view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher;

            eventDispatcher.DispatchEvent(
                new ReactImageLoadEvent(
                    view.GetTag(),
                    (int)status.LoadStatus,
                    status.Metadata.Uri,
                    status.Metadata.Width,
                    status.Metadata.Height));
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

            var disposable = default(SerialDisposable);
            if (!_disposables.TryGetValue(tag, out disposable))
            {
                disposable = new SerialDisposable();
                _disposables.Add(tag, disposable);
            }

            var image = new BitmapImage();
            image.BeginInit();
            image.CacheOption = BitmapCacheOption.OnLoad;
            if (BitmapImageHelpers.IsBase64Uri(source))
            {
                disposable.Disposable = image.GetStreamLoadObservable().Subscribe(
                    status => OnImageStatusUpdate(view, status),
                    _ => OnImageFailed(view));

                var stream = BitmapImageHelpers.GetStreamAsync(source);
                image.StreamSource = stream;
            }
            else
            {
                disposable.Disposable = image.GetUriLoadObservable().Subscribe(
                    status => OnImageStatusUpdate(view, status),
                    _ => OnImageFailed(view));

                image.UriSource = new Uri(source);
            }

            image.EndInit();
            imageBrush.ImageSource = image;
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
