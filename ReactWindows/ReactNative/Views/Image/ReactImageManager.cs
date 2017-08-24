using ImagePipeline.Core;
using ImagePipeline.Request;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using ReactNative.Modules.Image;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The view manager responsible for rendering native images.
    /// </summary>
    public class ReactImageManager : SimpleViewManager<Border>
    {
        private readonly Dictionary<int, ReactImageData> _imageData =
            new Dictionary<int, ReactImageData>();

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
                GetImageData(view).SetSources(sources);

                if (double.IsNaN(view.Width) || double.IsNaN(view.Height))
                {
                    // If we need to choose from multiple URIs but the size is not yet set, wait for layout pass
                    return;
                }

                SetUriFromMultipleSources(view);
            }
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
        /// Set the border radius of the <see cref="ReactRootView"/>.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="radius">The border radius value.</param>
        [ReactProp("borderRadius")]
        public void SetBorderRadius(Border view, double radius)
        {
            view.CornerRadius = new CornerRadius(radius);
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
                else if (data.SourceUri != null)
                {
                    UpdateImage(view);
                }
            }
        }

        /// <summary>
        /// Set the background color of the image view.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        /// <param name="color">The background color value.</param>
        [ReactProp("backgroundColor")]
        public void SetBackgroundColor(Border view, uint? color)
        {
            var data = GetImageData(view);
            if (data.BackgroundColor != color)
            {
                data.BackgroundColor = color;
                if (data.SourceUri != null)
                {
                    UpdateImage(view);
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
            base.OnDropViewInstance(reactContext, view);

            var tag = view.GetTag();
            _imageData[tag].Dispose();
            _imageData.Remove(tag);
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

        private void OnImageStatusUpdate(Border view, ImageLoadStatus status, ImageMetadata metadata)
        {
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
                var image = default(BitmapSource);
                var uri = new Uri(source);
                var builder = ImageRequestBuilder.NewBuilderWithSource(uri);
                var imageData = GetImageData(view);

                // Apply postprocessor if Composition isn't supported.
                if (imageData.BackgroundColor != null || 
                    (imageData.TintColor != null && !EffectManager.IsCompositionSupported))
                {
                    builder.SetPostprocessor(new ColorFilterPostprocessor(
                        imageData.TintColor, imageData.BackgroundColor));

                    image = await imagePipeline.FetchDecodedBitmapImageAsync(builder.Build());
                }
                else
                {
                    // Remote images
                    if (source.StartsWith("http:") || source.StartsWith("https:"))
                    {
                        image = await imagePipeline.FetchEncodedBitmapImageAsync(uri);                   
                    }
                    else // Base64 or local images
                    {
                        image = await imagePipeline.FetchDecodedBitmapImageAsync(ImageRequest.FromUri(uri));
                    }
                }

                var metadata = new ImageMetadata(source, image.PixelWidth, image.PixelHeight);
                OnImageStatusUpdate(view, ImageLoadStatus.OnLoad, metadata);
                imageBrush.ImageSource = image;
                OnImageStatusUpdate(view, ImageLoadStatus.OnLoadEnd, metadata);
                imageData.SourceUri = source;
            }
            catch
            {
                OnImageFailed(view);
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
            var tag = view.GetTag();
            var data = default(ReactImageData);
            if (!_imageData.TryGetValue(tag, out data))
            {
                data = new ReactImageData(view);
                _imageData.Add(tag, data);
            }

            return data;
        }

        private async void UpdateImage(Border view)
        {
            var imagePipeline = ImagePipelineFactory.Instance.GetImagePipeline();
            var imageData = GetImageData(view);
            var builder = ImageRequestBuilder.NewBuilderWithSource(new Uri(imageData.SourceUri));
            var tintColor = EffectManager.IsCompositionSupported ? null : imageData.TintColor;
            builder.SetPostprocessor(new ColorFilterPostprocessor(
                tintColor, imageData.BackgroundColor));

            var image = await imagePipeline.FetchDecodedBitmapImageAsync(builder.Build())
                .ConfigureAwait(false);

            DispatcherHelpers.RunOnDispatcher(() =>
            {
                var imageBrush = (ImageBrush)view.Background;
                imageBrush.ImageSource = image;
            });
        }
    }
}
