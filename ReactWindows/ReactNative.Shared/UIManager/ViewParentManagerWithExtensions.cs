using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    class ViewParentManagerWithExtensions : ViewManagerWithExtensions, IViewParentManager
    {
        private readonly IViewParentManager _viewManager;

        public ViewParentManagerWithExtensions(IViewParentManager viewManager, IReadOnlyList<IViewManagerExtension> extensions)
            : base(viewManager, extensions)
        {
            _viewManager = viewManager;
        }

        public bool NeedsCustomLayoutForChildren => _viewManager.NeedsCustomLayoutForChildren;

        public void AddView(DependencyObject parent, DependencyObject child, int index) =>
            _viewManager.AddView(parent, child, index);

        public DependencyObject GetChildAt(DependencyObject parent, int index) =>
            _viewManager.GetChildAt(parent, index);

        public int GetChildCount(DependencyObject parent) =>
            _viewManager.GetChildCount(parent);

        public void RemoveAllChildren(DependencyObject parent) =>
            _viewManager.RemoveAllChildren(parent);

        public void RemoveChildAt(DependencyObject parent, int index) =>
            _viewManager.RemoveChildAt(parent, index);
    }
}
