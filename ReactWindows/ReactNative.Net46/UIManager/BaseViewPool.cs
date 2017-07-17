using ReactNative.Pooling;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;

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
            obj.Opacity = 0;
            AutomationProperties.SetAutomationId(obj, "");
            AutomationProperties.SetName(obj, "");
            Panel.SetZIndex(obj, 0);
            obj.Effect = null;
            FreeCore(obj);
            base.Free(obj);
        }

        protected virtual void FreeCore(TFrameworkElement element)
        {
        }
    }
}
