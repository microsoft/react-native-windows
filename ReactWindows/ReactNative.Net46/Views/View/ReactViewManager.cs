using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Data;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.View;

namespace ReactNative.Net46.Views.View
{
    /// <summary>
    /// WPF-specifics of the RCTView implementation.
    /// </summary>
    public class ReactViewManager: BaseViewManager
    {
        /// <summary>
        /// Enables the Canvas as a drop target.
        /// </summary>
        [ReactProp("allowDrop")]
        public void SetAllowDrop(BorderedCanvas view, bool allowDrop)
        {
            throw new NotImplementedException("The allowDrop property is not available on this platform");
        }
    }
}
