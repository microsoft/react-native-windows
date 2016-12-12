using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System.Windows.Controls;
using System.Windows.Media;

namespace ReactNative.Views.Progress
{
    class ReactProgressBarViewManager : BaseViewManager<ProgressBar, ProgressBarShadowNode>
    {
        public override string Name
        {
            get
            {
                return "WindowsProgressBar";
            }
        }

        [ReactProp("indeterminate")]
        public void SetIndeterminate(ProgressBar view, bool value)
        {
            view.SetCurrentValue(ProgressBar.IsIndeterminateProperty, value);
        }

        [ReactProp("progress")]
        public void SetProgress(ProgressBar view, double value)
        {
            view.SetCurrentValue(ProgressBar.IsIndeterminateProperty, false);
            view.SetCurrentValue(System.Windows.Controls.Primitives.RangeBase.ValueProperty, value);
        }

        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(ProgressBar view, uint? color)
        {
            view.SetCurrentValue(Control.ForegroundProperty, color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null);
        }

        public override ProgressBarShadowNode CreateShadowNodeInstance()
        {
            return new ProgressBarShadowNode();
        }

        public override void UpdateExtraData(ProgressBar root, object extraData)
        {
        }

        protected override ProgressBar CreateViewInstance(ThemedReactContext reactContext)
        {
            return new ProgressBar();
        }
    }
}
