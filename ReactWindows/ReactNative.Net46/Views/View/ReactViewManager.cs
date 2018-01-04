using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;

namespace ReactNative.Views.View
{
    /// <summary>
    /// WPF-specifics of the RCTView implementation.
    /// </summary>
    public class ReactViewManager: ReactViewManagerBase
    {
        /// <summary>
        /// Enables the Canvas as a drop target.
        /// </summary>
        [ReactProp("allowDrop")]
        public void SetAllowDrop(BorderedCanvas view, bool allowDrop)
        {
            // not implemented for WPF
        }
    }
}
