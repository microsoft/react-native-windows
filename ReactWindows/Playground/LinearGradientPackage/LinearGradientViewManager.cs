using ReactNative.UIManager;
using ReactNative.Views.View;
using Windows.UI.Xaml.Controls;

namespace BV.LinearGradient
{
    public class LinearGradientViewManager : BorderedViewParentManager<LinearGradientContentControl>
    {
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "BVLinearGradient";
            }
        }

        /// <summary>
        /// Set the gradient direction line start point.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="start">The</param>
        [ReactProperty("start")]
        public void SetStart(LinearGradientContentControl view, double[] start)
        {
            view.SetStart(start);
        }

        /// <summary>
        /// Set the gradient direction line end point.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="end">The</param>
        [ReactProperty("end")]
        public void SetEnd(LinearGradientContentControl view, double[] end)
        {
            view.SetEnd(end);
        }

        /// <summary>
        /// Set the gradient colors.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="colors">The</param>
        [ReactProperty("colors")]
        public void SetColors(LinearGradientContentControl view, uint[] colors)
        {
            view.SetColors(colors);
        }

        /// <summary>
        /// Set the offset locations for the gradient stops.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="locations">The</param>
        [ReactProperty("locations")]
        public void SetLocations(LinearGradientContentControl view, double[] locations)
        {
            view.SetLocations(locations);
        }

        /// <summary>
        /// Creates a new view instance.
        /// </summary>
        /// <param name="reactContext">The react context.</param>
        /// <returns>The view instance.</returns>
        protected override LinearGradientContentControl CreateViewInstance(ThemedReactContext reactContext)
        {
            return new LinearGradientContentControl(new Canvas());
        }
    }
}
