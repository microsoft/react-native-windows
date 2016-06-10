using ReactNative.Modules.Image;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices.WindowsRuntime;
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
            var source = ((Tuple<string, Color?, Color?>)extraData).Item1;
            var tintColor = ((Tuple<string, Color?, Color?>)extraData).Item2;
            var backgroundColor = ((Tuple<string, Color?, Color?>)extraData).Item3;

            var loader = view.GetReactContext().GetNativeModule<ImageLoaderModule>();
            var eventDispatcher = view.GetReactContext().GetNativeModule<UIManagerModule>().EventDispatcher;

            eventDispatcher.DispatchEvent(
                new ReactImageLoadEvent(view.GetTag(),
                                        ReactImageLoadEvent.OnLoadStart));

            var image = loader.QueryImage(source);

            if (image != null)
            {             
                eventDispatcher.DispatchEvent(
                    new ReactImageLoadEvent(view.GetTag(),
                                            ReactImageLoadEvent.OnLoad));

                if (tintColor != null || backgroundColor != null)
                {
                    SetColors(view, image, source, tintColor, backgroundColor);
                }
                else
                {
                    ((ImageBrush)view.Background).ImageSource = image;
                    loader.FreeReference(source);
                }

                eventDispatcher.DispatchEvent(
                    new ReactImageLoadEvent(view.GetTag(),
                                            ReactImageLoadEvent.OnLoadEnd));
            }
            else
            {
                loader.LoadImage(source, this, Tuple.Create(view, tintColor, backgroundColor));

                eventDispatcher.DispatchEvent(
                    new ReactImageLoadEvent(view.GetTag(),
                                            ReactImageLoadEvent.OnLoad));
            }
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

        internal void ImageLoaded(object data, bool loaded, string source)
        {
            var view = ((Tuple<Border, Color?, Color?>)data).Item1;
            var tintColor = ((Tuple<Border, Color?, Color?>)data).Item2;
            var backgroundColor = ((Tuple<Border, Color?, Color?>)data).Item3;
            var loader = view.GetReactContext().GetNativeModule<ImageLoaderModule>();

            view.GetReactContext().GetNativeModule<UIManagerModule>().
                EventDispatcher.DispatchEvent(
                    new ReactImageLoadEvent(view.GetTag(),
                                            ReactImageLoadEvent.OnLoadEnd));

            if (loaded)
            {
                var image = loader.GetImage(source);

                if (tintColor != null || backgroundColor != null)
                {
                    SetColors(view, image, source, tintColor, backgroundColor);
                }
                else
                {
                    ((ImageBrush)view.Background).ImageSource = image;
                    loader.FreeReference(source);
                }
            }
        }

        internal void DataLoaded(object data, string source)
        {
            var view = ((Tuple<Border, Color?, Color?>)data).Item1;
            var tintColor = ((Tuple<Border, Color?, Color?>)data).Item2;
            var backgroundColor = ((Tuple<Border, Color?, Color?>)data).Item3;
            var loader = view.GetReactContext().GetNativeModule<ImageLoaderModule>();

            var image = loader.GetImage(source);
            var pixels = loader.GetPixelData(source);

            SetPixels(view, image, tintColor, backgroundColor, pixels, source);
        }

        private void SetColors(Border view, BitmapImage image, string source, Color? tintColor, Color? backgroundColor)
        {
            var loader = view.GetReactContext().GetNativeModule<ImageLoaderModule>();
            var pixelData = loader.GetPixelData(source);

            if (pixelData != null)
            {
                SetPixels(view, image, tintColor, backgroundColor, pixelData, source);
            }
            else
            {
                loader.LoadImagePixelData(source, this, Tuple.Create(view, tintColor, backgroundColor));
            }
        }

        private async void SetPixels(
            Border view, 
            BitmapImage image, 
            Color? tintColor, 
            Color? backgroundColor, 
            byte[] pixelData, 
            string source)
        {
            var pixels = new byte[pixelData.Length];
            pixelData.CopyTo(pixels, 0);

            for (var i = 3; i < pixels.Length; i += 4) // 0 = B, 1 = G, 2 = R, 3 = A
            {
                if (tintColor != null)
                {
                    if (pixels[i] != 0)
                    {
                        pixels[i - 3] = (byte)tintColor?.B;
                        pixels[i - 2] = (byte)tintColor?.G;
                        pixels[i - 1] = (byte)tintColor?.R;
                    }
                }
                if (backgroundColor != null)
                {
                    var frontAlpha = (double)pixels[i] / 255;
                    var backAlpha = (double)backgroundColor?.A / 255;

                    pixels[i - 3] = ColorValue(pixels[i - 3], frontAlpha, (double)backgroundColor?.B, backAlpha);
                    pixels[i - 2] = ColorValue(pixels[i - 2], frontAlpha, (double)backgroundColor?.G, backAlpha);
                    pixels[i - 1] = ColorValue(pixels[i - 1], frontAlpha, (double)backgroundColor?.R, backAlpha);
                    pixels[i] = AlphaValue(pixels[i], (double)backgroundColor?.A);
                }
            }

            var writableBitmap = new WriteableBitmap(image.PixelWidth, image.PixelHeight);

            // Open a stream to copy the image contents to the WriteableBitmap's pixel buffer 
            using (System.IO.Stream writeStream = writableBitmap.PixelBuffer.AsStream())
            {
                await writeStream.WriteAsync(pixels, 0, pixels.Length);
            }

            ((ImageBrush)view.Background).ImageSource = writableBitmap;

            view.GetReactContext().GetNativeModule<ImageLoaderModule>().FreeReference(source);
        }

        private static byte ColorValue(double frontColor, double frontAlpha, double backColor, double backAlpha)
        {
            return (byte)((frontColor * frontAlpha + backColor * backAlpha * (1 - frontAlpha)) / 
                                (frontAlpha + backAlpha * (1 - frontAlpha)));
        }

        private static byte AlphaValue(double frontAlpha, double backAlpha)
        {
            return (byte)(frontAlpha + backAlpha * (1 - (frontAlpha / 255)));
        }
    }
}
