using ReactNative.Modules.Image;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
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
                    ((ImageBrush)view.Background).ImageSource = image.Image;
                    image.Dispose();
                }

                eventDispatcher.DispatchEvent(
                    new ReactImageLoadEvent(view.GetTag(),
                                            ReactImageLoadEvent.OnLoadEnd));
            }
            else
            {
                loader.LoadImage(source, Tuple.Create(this, view, tintColor, backgroundColor));

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
            var view = ((Tuple<ReactImageManager, Border, Color?, Color?>)data).Item2;
            var tintColor = ((Tuple<ReactImageManager, Border, Color?, Color?>)data).Item3;
            var backgroundColor = ((Tuple<ReactImageManager, Border, Color?, Color?>)data).Item4;

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
                    ((ImageBrush)view.Background).ImageSource = image.Image;
                    image.Dispose();
                }
            }
        }

        internal void DataLoaded(object data, string source)
        {
            var view = ((Tuple<ReactImageManager, Border, Color?, Color?>)data).Item2;
            var tintColor = ((Tuple<ReactImageManager, Border, Color?, Color?>)data).Item3;
            var backgroundColor = ((Tuple<ReactImageManager, Border, Color?, Color?>)data).Item4;

            var loader = view.GetReactContext().GetNativeModule<ImageLoaderModule>();
            var image = loader.GetImage(source);

            ImageHelpers.SetPixels(view, image, tintColor, backgroundColor);
        }

        private void SetColors(Border view, ImageReference image, string source, Color? tintColor, Color? backgroundColor)
        {
            var loader = view.GetReactContext().GetNativeModule<ImageLoaderModule>();

            if (image.PixelData != null)
            {
                ImageHelpers.SetPixels(view, image, tintColor, backgroundColor);
            }
            else
            {
                loader.LoadPixelDataAsync(source, Tuple.Create(this, view, tintColor, backgroundColor));
            }
        }       
    }
}
