using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;

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
            return new ProgressBar { IsIndeterminate = true };
        }
        


    }
}
