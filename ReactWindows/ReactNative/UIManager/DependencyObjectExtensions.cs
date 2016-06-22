using ReactNative.Bridge;
using System;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml;

namespace ReactNative.UIManager
{
    static class DependencyObjectExtensions
    {
        private static readonly ConditionalWeakTable<DependencyObject, DependencyObjectData> s_properties =
            new ConditionalWeakTable<DependencyObject, DependencyObjectData>();

        public static void SetTag(this DependencyObject view, int tag)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            s_properties.GetOrCreateValue(view).Tag = tag;
        }

        public static int GetTag(this DependencyObject view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            var elementData = default(DependencyObjectData);
            if (!s_properties.TryGetValue(view, out elementData) || !elementData.Tag.HasValue)
            {
                throw new InvalidOperationException("Could not get tag for view.");
            }

            return elementData.Tag.Value;
        }

        public static bool HasTag(this DependencyObject view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            var elementData = default(DependencyObjectData);
            return s_properties.TryGetValue(view, out elementData) && elementData.Tag.HasValue;
        }

        public static void SetReactContext(this DependencyObject view, ThemedReactContext context)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            s_properties.GetOrCreateValue(view).Context = context;
        }

        public static ThemedReactContext GetReactContext(this DependencyObject view)
        {
            if (view == null)
                throw new ArgumentNullException(nameof(view));

            var elementData = default(DependencyObjectData);
            if (!s_properties.TryGetValue(view, out elementData) || !elementData.Tag.HasValue)
            {
                throw new InvalidOperationException("Could not get tag for view.");
            }

            return elementData.Context;
        }

        public static T As<T>(this DependencyObject view)
            where T : DependencyObject
        {
            var convertedView = view as T;
            if (convertedView == null)
            {
                throw new ArgumentOutOfRangeException($"Child of type '{view.GetType()}' is not assignable to '{typeof(T)}'.");
            }
            return convertedView;
        }

        class DependencyObjectData
        {
            public ThemedReactContext Context { get; set; }

            public int? Tag { get; set; }
        }
    }
}
