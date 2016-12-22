using Microsoft.Toolkit.Uwp.UI;
using Newtonsoft.Json.Linq;
using ReactNative.Collections;
using ReactNative.Modules.Image;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reactive.Disposables;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The view manager responsible for rendering native images.
    /// </summary>
    public class ReactImageManager : BaseViewManager<Border, ReactImageShadowNode>
    {
        private readonly Dictionary<int, SerialDisposable> _disposables =
            new Dictionary<int, SerialDisposable>();

        private readonly Dictionary<int, List<KeyValuePair<string, double>>> _imageSources =
            new Dictionary<int, List<KeyValuePair<string, double>>>();

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
        /// This method should return the <see cref="ReactImageShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view. 
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactImageShadowNode CreateShadowNodeInstance()
        {
            return new ReactImageShadowNode();
        }

        /// <summary>
        /// Implement this method to receive optional extra data enqueued from
        /// the corresponding instance of <see cref="ReactShadowNode"/> in
        /// <see cref="ReactShadowNode.OnCollectExtraUpdates"/>.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="extraData">The extra data.</param>
        public override void UpdateExtraData(Border view, object extraData)
        {
            Tuple<JArray, Color?, Color?> imageExtraData = (Tuple<JArray, Color?, Color?>)extraData;
            var imageBrush = (ImageBrush)view.Background;

            OnImageStatusUpdate(view, ImageLoadStatus.OnLoadStart, new ImageMetadata());
//            OnImageStatusUpdate(view, new ImageStatusEventData(ImageLoadStatus.OnLoadStart));

            var sources = imageExtraData.Item1;
            var tintColor = imageExtraData.Item2;
            var backgroundColor = imageExtraData.Item3;

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
                SetUriFromSingleSource(view, uri, tintColor, backgroundColor);
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

                var uriToLoad = ChooseUriFromMultipleSources(view);
                SetUriFromSingleSource(view, uriToLoad, tintColor, backgroundColor);
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
            base.OnDropViewInstance(reactContext, view);

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
        ///
        /*
        public override void SetDimensions(Border view, Dimensions dimensions)
        {
            base.SetDimensions(view, dimensions);
            SetUriFromMultipleSources(view);
        }
        */
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
        private async void SetUriFromSingleSource(Border view, string source, Color? tintColor, Color? backgroundColor)
        {
            var imageBrush = (ImageBrush)view.Background;
            var tag = view.GetTag();

            var disposable = default(SerialDisposable);
            if (!_disposables.TryGetValue(tag, out disposable))
            {
                disposable = new SerialDisposable();
                _disposables.Add(tag, disposable);
            }

            if (BitmapImageHelpers.IsBase64Uri(source))
            {
                var image = new BitmapImage();

                disposable.Disposable = image.GetStreamLoadObservable().Subscribe(
                    status => OnImageStatusUpdate(view, status.LoadStatus, status.Metadata),
                    _ => OnImageFailed(view));

                using (var stream = await BitmapImageHelpers.GetStreamAsync(source))
                {
                    await image.SetSourceAsync(stream);
                }

                imageBrush.ImageSource = image;
            }
            else if (BitmapImageHelpers.IsHttpUri(source))
            {
                OnImageStatusUpdate(view, ImageLoadStatus.OnLoadStart, default(ImageMetadata));
                try
                {
                    var image = await ImageCache.Instance.GetFromCacheAsync(new Uri(source), true);
                    var metadata = new ImageMetadata(source, image.PixelWidth, image.PixelHeight);
                    OnImageStatusUpdate(view, ImageLoadStatus.OnLoad, metadata);
                    if (tintColor == null && backgroundColor == null)
                        imageBrush.ImageSource = image;
                    else
                    {
                        using (var stream = await CreateStreamFromHttpUri(source))
                        {
                            imageBrush.ImageSource = await ColorizeBitmap(stream, tintColor, backgroundColor);
                        }
                    }
                    OnImageStatusUpdate(view, ImageLoadStatus.OnLoadEnd, metadata);
                }
                catch
                {
                    OnImageFailed(view);
                }
            }
            else
            {
                if (tintColor != null || backgroundColor != null)
                {
                    using (var stream = await CreateStreamFromAppUri(source))
                    {
                        imageBrush.ImageSource = await ColorizeBitmap(stream, tintColor, backgroundColor);
                    }
                }
                else
                {
                    var image = new BitmapImage();
                    disposable.Disposable = image.GetUriLoadObservable().Subscribe(
                    status => OnImageStatusUpdate(view, status.LoadStatus, status.Metadata),
                        _ => OnImageFailed(view));
                    image.UriSource = new Uri(source);
                    imageBrush.ImageSource = image;
                }
            }
        }

        private static async Task<IRandomAccessStream> CreateStreamFromHttpUri(string source)
        {
            /* We could use RandomAccessStreamReference.CreateFromUri(source).OpenReadAsync()
             *  if it didn't mysteriously occasionally fail with dev server.
             *  Therefore we have to climb the tree with feet first.
             */
            var response = await System.Net.HttpWebRequest.CreateHttp(source).GetResponseAsync();
            using (var responseStream = response.GetResponseStream())
            {
                using (var memStream = new MemoryStream())
                {
                    await responseStream.CopyToAsync(memStream);
                    return memStream.AsRandomAccessStream();
                }
            }
        }

        private static async Task<IRandomAccessStream> CreateStreamFromAppUri(string source)
        {
            return await RandomAccessStreamReference.CreateFromUri(new Uri(source)).OpenReadAsync();
        }

        // Applies tintcolor and backgroundcolor for an image.
        // Cannot use the stored BitmapImage since there is no way to get raw data.
        private static async Task<WriteableBitmap> ColorizeBitmap(IRandomAccessStream stream, Color? tintColor, Color? backgroundColor)
        {
            var decoder = await BitmapDecoder.CreateAsync(stream);

            var pixelData = await decoder.GetPixelDataAsync(
                BitmapPixelFormat.Bgra8, // WriteableBitmap uses BGRA format 
                BitmapAlphaMode.Premultiplied,
                new BitmapTransform(),
                ExifOrientationMode.RespectExifOrientation,
                ColorManagementMode.DoNotColorManage
            );

            return await BitmapImageHelpers.ColorizePixelData(decoder.PixelWidth, decoder.PixelHeight, pixelData.DetachPixelData(), tintColor, backgroundColor);
        }

        private void Bi_ImageOpened(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Chooses the uri with the size closest to the target image size. Must be called only after the
        /// layout pass when the sizes of the target image have been computed, and when there are at least
        /// two sources to choose from.
        /// </summary>
        /// <param name="view">The image view instance.</param>
        private string ChooseUriFromMultipleSources(Border view)
        {
            var sources = default(List<KeyValuePair<string, double>>);
            if (_imageSources.TryGetValue(view.GetTag(), out sources))
            {
                var targetImageSize = view.Width * view.Height;
                var bestResult = sources.LocalMin((s) => Math.Abs(s.Value - targetImageSize));
                return bestResult.Key;
            }
            return "";
        }
    }
}
