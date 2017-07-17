using ReactNative.Pooling;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace ReactNative.UIManager
{
    class BaseViewPool<TFrameworkElement> : ObjectPool<TFrameworkElement>
        where TFrameworkElement : FrameworkElement
    {

        /// <remarks>
        /// Not using System.Func{T} because this file is linked into the (debugger) Formatter,
        /// which does not have that type (since it compiles against .NET 2.0).
        /// </remarks>
        public delegate TFrameworkElement ViewFactory();

        public BaseViewPool(ViewFactory factory)
            : base(new Factory(factory))
        {
        }

        public BaseViewPool(ViewFactory factory, int size)
            : base(new Factory(factory), size)
        {
        }

        public new void Free(TFrameworkElement obj)
        {
            obj.RenderTransform = null;
            obj.Projection = null;
            obj.Opacity = 1;
            WinRTXamlToolkit.Controls.Extensions.FrameworkElementExtensions.SetClipToBounds(obj, false);
            AutomationProperties.SetAutomationId(obj, "");
            AutomationProperties.SetLiveSetting(obj, AutomationLiveSetting.Off);
            AutomationProperties.SetName(obj, "");
            Canvas.SetZIndex(obj, 0);
            FreeCore(obj);
            base.Free(obj);
        }

        protected virtual void FreeCore(TFrameworkElement element)
        {
        }
    }
}
