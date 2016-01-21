using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The view manager responsible for rendering native <see cref="ImageControl"/>.
    /// </summary>
    public class ReactImageView : SimpleViewManager<BorderedContentControl>
    {
        private const string ReactClass = "RCTImageView";
        private const string PROP_SOURCE = "src";

        public override string Name
        {
            get
            {
                return ReactClass;
            }
        }

        public override IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants
        {
            get
            {
                return new Dictionary<string, object>
                    {
                        { "topLoad", new Dictionary<string, object> { { "registrationName", "onLoad" } } },
                        { "topLoadEnd", new Dictionary<string, object> { { "registrationName", "onLoadEnd" } } },
                        { "topLoadStart", new Dictionary<string, object> { { "registrationName", "onLoadStart" } } },
                    };
            }
        }

        protected override BorderedContentControl CreateViewInstanceCore(ThemedReactContext reactContext)
        {
            return new BorderedContentControl();
        }

        /// <summary>
        /// Sets the <see cref="BrushImage"/> source for the background of a <see cref="BorderedContentControl"/>.
        /// </summary>
        /// <param name="view">The text input box control.</param>
        /// <param name="degrees">The text alignment.</param>
        [ReactProperty(PROP_SOURCE)]
        public void SetSource(BorderedContentControl view, string source)
        {
            var imageSrcURL = default(Uri);

            if (source != null)
            {
                if(!Uri.TryCreate(source, UriKind.Absolute, out imageSrcURL))
                {
                    imageSrcURL = new Uri("ms-appx://" + source);
                }

                if (imageSrcURL != null)
                {
                    var backgroundImage = new ImageBrush()
                    {
                        ImageSource = new BitmapImage(imageSrcURL)
                    };

                    view.Background = backgroundImage;
                }
            }
        }
    }
}
