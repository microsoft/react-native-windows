using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.TextInput;

namespace ReactNative.Views.Progressbar
{
    /// <summary>
    /// 
    /// </summary>
    public class ReactProgressBarViewManager : BaseViewManager<ProgressBar, ReactProgressBarShadowNode>
    {
        private const string ReactClass = "WindowsProgressBar";

        public override string Name => ReactClass;

        public override ReactProgressBarShadowNode CreateShadowNodeInstance()
        {
            return new ReactProgressBarShadowNode();
        }

        public override void UpdateExtraData(ProgressBar root, object extraData)
        {
            //throw new NotImplementedException();
        }

        protected override ProgressBar CreateViewInstance(ThemedReactContext reactContext)
        {
            return new ProgressBar();
        }


        [ReactProp("indeterminate", DefaultBoolean = false)]
        public void SetIndeterminate(ProgressBar view, bool value)
        {
            view.IsIndeterminate = value;
        }

        [ReactProp("progress", DefaultDouble = 0)]
        public void SetProgress(ProgressBar view, double value)
        {
            view.Value = value * 100.0;//xaml progressbar is 0-100, but RN progressbar is 0-1.
        }

        /// <summary>
        /// Sets the foreground for the node.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="color">The masked color value.</param>
        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(ProgressBar view, uint? color)
        {
            view.Foreground = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }
    }
}
