// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ImagePipeline.Core;
using Newtonsoft.Json.Linq;
using ReactNative.Collections;
using ReactNative.Modules.Image;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using Windows.ApplicationModel.Core;
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
        private readonly ConcurrentDictionary<Border, ReactImageData> _imageData =
            new ConcurrentDictionary<Border, ReactImageData>();

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
                    {
                        "topError",
                        new JObject
                        {
                            { "registrationName", "onError" }
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
            if (resizeMode != null)
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
                GetImageData(view).SetSources(sources);

                if (double.IsNaN(view.Width) || double.IsNaN(view.Height))
                {
                    // If we need to choose from multiple URIs but the size is not yet set,
                    // wait for layout pass
                    return;
                }

                SetUriFromMultipleSources(view);
            }
        }

        /// <summary>
        /// Enum values correspond to positions of prop names in ReactPropGroup attribute
        /// applied to <see cref="SetBorderRadius(Border, int, double)"/>
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
        /// Set the border radius of the <see cref="ReactRootView"/>.
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
        public void SetBorderRadius(Border view, int index, double radius)
        {
            var cornerRadius = view.CornerRadius;
            switch ((Radius)index)
            {
                case Radius.All:
                    cornerRadius = new CornerRadius(radius);
                    break;
                case Radius.TopLeft:
                    cornerRadius.TopLeft = radius;
                    break;
                case Radius.TopRight:
                    cornerRadius.TopRight = radius;
                    break;
                case Radius.BottomLeft:
                    cornerRadius.BottomLeft = radius;
                    break;
                case Radius.BottomRight:
                    cornerRadius.BottomRight = radius;
                    break;
            }

            view.CornerRadius = cornerRadius;
        }

        /// <summary>
        /// Set the blur radius of the image view.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="blurRadius">The blur radius value.</param>
        [ReactProp("blurRadius")]
        public void SetBlurRadius(Border view, double blurRadius)
        {
            var data = GetImageData(view);
            if (data.BlurRadius != blurRadius)
            {
                data.BlurRadius = blurRadius;
                if (EffectManager.IsCompositionSupported)
                {
                    data.EffectManager.SetBlurRadius(blurRadius);
                }
                else
                {
                    Debug.WriteLine("Blur radius isn't supported.");
                }
            }
        }

        /// <summary>
        /// Set the tint color of the image view.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="color">The tint color value.</param>
        [ReactProp("tintColor")]
        public void SetTintColor(Border view, uint? color)
        {
            var data = GetImageData(view);
            if (data.TintColor != color)
            {
                data.TintColor = color;
                if (EffectManager.IsCompositionSupported)
                {
                    data.EffectManager.SetTintColor(color.Value);
                }
                else
                {
                    Debug.WriteLine("Tint color isn't supported.");
                }
            }
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
        /// <param name="index">The prop index.</param>
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
            base.OnDropViewInstance(reactContext, view);

            _imageData[view].Dispose();
            _imageData.TryRemove(view, out _);
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

        private void OnImageFailed(Border view, Exception e)
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
                    e.Message));

            eventDispatcher.DispatchEvent(
                new ReactImageLoadEvent(
                    view.GetTag(),
                    ReactImageLoadEvent.OnLoadEnd));

        }

        private void OnImageStatusUpdate(Border view, ImageLoadStatus status, ImageMetadata metadata)
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
                    (int)status,
                    metadata.Uri,
                    metadata.Width,
                    metadata.Height));
        }

        /// <summary>
        /// Set the source URI of the image.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="source">The source URI.</param>
        private async void SetUriFromSingleSource(Border view, string source)
        {
            var imageBrush = (ImageBrush)view.Background;
            OnImageStatusUpdate(view, ImageLoadStatus.OnLoadStart, default(ImageMetadata));
            try
            {
                var imagePipeline = ImagePipelineFactory.Instance.GetImagePipeline();
                var dispatcher = CoreApplication.GetCurrentView().Dispatcher;
                var image = await imagePipeline.FetchEncodedBitmapImageAsync(new Uri(source), default(CancellationToken), dispatcher);
                var imageData = GetImageData(view);
                var metadata = new ImageMetadata(source, image.PixelWidth, image.PixelHeight);

                OnImageStatusUpdate(view, ImageLoadStatus.OnLoad, metadata);
                imageBrush.ImageSource = image;
                OnImageStatusUpdate(view, ImageLoadStatus.OnLoadEnd, metadata);
                imageData.SourceUri = source;
            }
            catch (Exception e)
            {
                OnImageFailed(view, e);
            }
        }

        /// <summary>
        /// Chooses the uri with the size closest to the target image size. Must be called only after the
        /// layout pass when the sizes of the target image have been computed, and when there are at least
        /// two sources to choose from.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        private void SetUriFromMultipleSources(Border view)
        {
            var sources = GetImageData(view).Sources;
            if (sources != default(List<KeyValuePair<string, double>>))
            {
                var targetImageSize = view.Width * view.Height;
                var bestResult = sources.LocalMin((s) => Math.Abs(s.Value - targetImageSize));
                SetUriFromSingleSource(view, bestResult.Key);
            }
        }

        private ReactImageData GetImageData(Border view)
        {
            var data = default(ReactImageData);
            if (!_imageData.TryGetValue(view, out data))
            {
                data = new ReactImageData(view);
                _imageData.AddOrUpdate(view, data, (k, v) => data);
            }

            return data;
        }

        class ReactImageData : IDisposable
        {
            private List<KeyValuePair<string, double>> _sources;

            public List<KeyValuePair<string, double>> Sources
            {
                get
                {
                    return _sources;
                }
            }

            public string SourceUri { get; set; }

            public EffectManager EffectManager { get; private set; }

            public uint? TintColor { get; set; }

            public double? BlurRadius { get; set; }

            public ReactImageData(FrameworkElement element)
            {
                EffectManager = new EffectManager(element);
            }

            public void SetSources(JArray sources)
            {
                if (_sources != default(List<KeyValuePair<string, double>>))
                {
                    _sources.Clear();
                }
                else
                {
                    _sources = new List<KeyValuePair<string, double>>(sources.Count);
                }

                foreach (var source in sources)
                {
                    var sourceData = (JObject)source;
                    _sources.Add(
                        new KeyValuePair<string, double>(
                            sourceData.Value<string>("uri"),
                            sourceData.Value<double>("width") * sourceData.Value<double>("height")));
                }

                _sources.Sort((p1, p2) => p1.Value.CompareTo(p2.Value));
            }

            /// <summary>
            /// Disposes the <see cref="EffectManager"/>.
            /// </summary>
            public void Dispose()
            {
                EffectManager.Dispose();
            }
        }
    }
}
