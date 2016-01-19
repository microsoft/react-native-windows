using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;
using ImageControl = Windows.UI.Xaml.Controls.Image;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// The view manager responsible for rendering native <see cref="ImageControl"/>.
    /// </summary>
    public class ReactImageView : SimpleViewManager<ImageControl>
    {
        private const string ReactClass = "Image";

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
                        { "otherSelectionChange", 42 },
                        { "topSelectionChange", new Dictionary<string, object> { { "registrationName", 42 } } },
                        { "topLoadingStart", new Dictionary<string, object> { { "foo", 42 } } },
                        { "topLoadingError", 42 },
                    };
            }
        }

        protected override ImageControl CreateViewInstanceCore(ThemedReactContext reactContext)
        {
            throw new NotImplementedException();
        }
    }
}
