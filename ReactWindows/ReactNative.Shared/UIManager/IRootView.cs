#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Interface for the root native view of a React Native application.
    /// </summary>
    public interface IRootView
    {
        /// <summary>
        /// Called when a child starts a native gesture.
        /// </summary>
        /// <param name="e">The event.</param>
        void OnChildStartedNativeGesture(RoutedEventArgs e);
    }
}
