#if WINDOWS_UWP
using Windows.Foundation;
using Windows.UI.Xaml;
#else
using System.Windows;
#endif
using Newtonsoft.Json.Linq;


namespace ReactNative.UIManager
{
    /// <summary>
    /// Interface consisting of methods which are relevant to views which delegate the
    /// applying of transforms to an external entity
    /// </summary>
    public interface ITransformControl
    {
        /// <summary>
        /// Sets the  <typeof name="FrameworkElement"/> transform 
        /// properties, based on the <see cref="JArray"/> object.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="transforms">The list of transforms.</param>
        void SetTransform(FrameworkElement view, JArray transforms);

        /// <summary>
        /// Gets the <typeof name="FrameworkElement"/> transform 
        /// properties, based on the <see cref="JObject"/> map.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <returns>Returns a <see cref="JArray"/> object</returns>
        JArray GetTransform(FrameworkElement view);
    }
}
