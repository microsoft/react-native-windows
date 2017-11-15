using ReactNative.Bridge;
using ReactNative.Reflection;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace ReactNative.Modules.Cursor
{
    class CursorModule : NativeModuleBase
    {
        public override string Name
        {
            get
            {
                return "Cursor";
            }
        }

        /// <summary>
        /// Set a mouse cursor
        /// </summary>
        /// <param name="cursor">The name of a system cursor (listed here: https://docs.microsoft.com/en-us/uwp/api/windows.ui.core.corecursortype ).</param>
        [ReactMethod]
        public void setCursor(string cursor)
        {
            var convertedCursor = EnumHelpers.Parse<CoreCursorType>(cursor);
            DispatcherHelpers.RunOnDispatcher(() =>
            {
                Window.Current.CoreWindow.PointerCursor = new CoreCursor(convertedCursor, 0);
            });
        }

        /// <summary>
        /// Set a mouse cursor
        /// </summary>
        /// <param name="cursor">The resource ID of the custom cursor</param>
        [ReactMethod]
        public void setCursor(uint cursor)
        {
            Window.Current.CoreWindow.PointerCursor = new CoreCursor(CoreCursorType.Custom, cursor);
        }
    }
}
