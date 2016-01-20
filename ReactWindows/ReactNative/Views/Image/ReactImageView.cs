using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;


namespace ReactNative.Views.Image
{
    /// <summary>
    /// The view manager responsible for rendering native <see cref="ImageControl"/>.
    /// </summary>
    public class ReactImageView : SimpleViewManager<BorderedContentControl>
    {
        private const string ReactClass = "RCTImageView";

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
            return new BorderedContentControl(new ReactPanel());
        }
    }
}
