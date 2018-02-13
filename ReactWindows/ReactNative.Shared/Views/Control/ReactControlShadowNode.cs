using Facebook.Yoga;
using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager.Annotations;
using System;
using static System.FormattableString;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Shadow node subclass for WindowsControl.
    /// </summary>
    public class ReactControlShadowNode : LayoutShadowNode
    {
        /// <summary>
        /// Instantiates a <see cref="ReactControlShadowNode"/>.
        /// </summary>
        public ReactControlShadowNode() :
            base(false, true)
        {
        }
    }
}
