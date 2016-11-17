using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Progress
{
    class ReactProgressBarViewManager : BaseViewManager<ProgressBar, ProgressBarShadowNode>
    {
        /// <summary>
        /// Instantiates the base class <see cref="ReactProgressBarViewManager"/>.
        /// </summary>
        public ReactProgressBarViewManager()
        {
        }

        /// <summary>
        /// Instantiates the base class <see cref="ReactProgressBarViewManager"/>.
        /// </summary>
        /// <param name="eventDispatcher">The event dispatcher to associate with this instance</param>
        public ReactProgressBarViewManager(IEventDispatcher eventDispatcher)
            : base(eventDispatcher)
        {
        }

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
            view.IsIndeterminate = value;
        }

        [ReactProp("progress")]
        public void SetProgress(ProgressBar view, double value)
        {
            view.Value = value;
        }

        [ReactProp(ViewProps.Color, CustomType = "Color")]
        public void SetColor(ProgressBar view, uint? color)
        {
            view.Foreground = color.HasValue
                ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
                : null;
        }

        public override ProgressBarShadowNode CreateShadowNodeInstance()
        {
            return new ProgressBarShadowNode();
        }

        public override void UpdateExtraData(ProgressBar root, object extraData)
        {
        }

        protected override ProgressBar CreateViewInstance()
        {
            return new ProgressBar();
        }
    }
}
