// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Runtime.CompilerServices;
using System.Threading;
using System;
#if WINDOWS_UWP
using ReactNative.Accessibility;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;
#else
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// Represents a Canvas with an optional Border inside.
    /// </summary>
#if WINDOWS_UWP
    public class BorderedCanvas : Canvas, IAccessible
#else
    public class BorderedCanvas : Canvas
#endif
    {
        private Border _border = null;

        /// <summary>
        /// The Border associated with this Canvas or null if it doesn't have a border.
        /// The Border is always the first element in the Children collection.
        /// </summary>
        private Border Border
        {
            get => _border;
            set
            {
                if (_border != null)
                {
                    throw new InvalidOperationException("The Canvas already has a Border");
                }

                _border = value;

                Children.Insert(0, _border);
            }
        }

        private class BorderProps
        {
            public Brush Brush;
        }

        private static readonly ThreadLocal<ConditionalWeakTable<BorderedCanvas, BorderProps>> s_borderProps =
            new ThreadLocal<ConditionalWeakTable<BorderedCanvas, BorderProps>>(
                () => new ConditionalWeakTable<BorderedCanvas, BorderProps>());

        /// <summary>
        /// Some border props do not affect appearance unless they are combined
        /// with other props. Such props are saved in the dictionary and applied
        /// later. This allows to not create the inner Border if it's not visible.
        /// </summary>
        private BorderProps GetBorderProps()
        {
            if (s_borderProps.Value.TryGetValue(this, out var props))
            {
                return props;
            }

            return null;
        }

        private BorderProps GetOrCreateBorderProps()
        {
            if (!s_borderProps.Value.TryGetValue(this, out var props))
            {
                props = new BorderProps();
                s_borderProps.Value.Add(this, props);
            }

            return props;
        }

        /// <summary>
        /// Canvas.Background supports flat backgrounds. Border.Background supports
        /// backgrounds with customizations, such as rounded corners. If the background
        /// is flat, it's set on Canvas. If it has cutomizations, it's transferred to Border.
        /// </summary>
        private void TransferBackgroundBrush()
        {
            if (base.Background != null && base.Background != _defaultBackgroundBrush && Border != null)
            {
                Border.Background = base.Background;
                base.Background = _defaultBackgroundBrush;
            }
        }

        private static readonly ThreadLocal<Brush> s_defaultBorderBrush = new ThreadLocal<Brush>(() => new SolidColorBrush(Colors.Black));

        /// <summary>
        /// Default brush for the view borders.
        /// </summary>
        protected Brush DefaultBorderBrush => s_defaultBorderBrush.Value;

        /// <summary>
        /// In WPF in order to be clickable (hit-test visible) the element needs to have background brush.
        /// This is why when the background and border brushes are set on the inner Border, the Canvas gets
        /// a transparent background brush.
        /// </summary>                
        protected readonly Brush _defaultBackgroundBrush
#if WINDOWS_UWP
            = null;
#else
            = new SolidColorBrush(Color.FromArgb(0, 0, 0, 0));
#endif

        /// <summary>
        /// Checks if the Canvas has a Border already.
        /// </summary>
        protected bool HasBorder()
        {
            return Border != null;
        }

        /// <summary>
        /// Adds a Border to a Canvas if it hasn't been added already.
        /// </summary>
        protected Border GetOrCreateBorder()
        {
            if (Border == null)
            {
                var borderProps = GetBorderProps();
                var borderBrush = borderProps?.Brush ?? DefaultBorderBrush;
                Border = new Border { BorderBrush = borderBrush };

                // Layout animations bypass SetDimensions, hence using XAML bindings.

                Border.SetBinding(FrameworkElement.WidthProperty, new Binding
                {
                    Source = this,
                    Path = new PropertyPath("Width")
                });

                Border.SetBinding(FrameworkElement.HeightProperty, new Binding
                {
                    Source = this,
                    Path = new PropertyPath("Height")
                });

                TransferBackgroundBrush();
            }

            return Border;
        }

        /// <summary>
        /// Gets or sets background brush of Canvas itself or of Border child as
        /// appropriate. This is the only supported method to access background
        /// of BorderedCanvas. DO NOT use Canvas.Background or Border.Background directly.
        /// </summary>
        public new Brush Background
        {
            get
            {
                if (Border == null)
                {
                    return base.Background;
                }
                else
                {
                    var border = GetOrCreateBorder();
                    return border.Background;
                }
            }
            set
            {
                if (Border == null)
                {
                    base.Background = value;
                }
                else
                {
                    var border = GetOrCreateBorder();
                    border.Background = value;
                    base.Background = base.Background ?? _defaultBackgroundBrush;
                }
            }
        }

        /// <summary>
        /// Gets or sets the border radius of the view.
        /// </summary>
        public CornerRadius BorderRadius
        {
            get
            {
                if (HasBorder())
                {
                    var border = GetOrCreateBorder();
                    return border.CornerRadius;
                }
                else
                {
                    return new CornerRadius();
                }
            }
            set
            {
                var border = GetOrCreateBorder();
                border.CornerRadius = value;
            }
        }

        /// <summary>
        /// Sets the background color of the view.
        /// </summary>
        public Color BackgroundColor
        {
            set => Background = new SolidColorBrush(value);
        }

        /// <summary>
        /// Set the border color of the view.
        /// </summary>
        public Brush BorderBrush
        {
            set
            {
                if (Border == null)
                {
                    GetOrCreateBorderProps().Brush = value;
                }
                else
                {
                    var border = GetOrCreateBorder();
                    border.BorderBrush = value ?? DefaultBorderBrush;
                }
            }
        }

        /// <summary>
        /// Sets the border thickness of the view.
        /// </summary>
        public Thickness BorderThickness
        {
            get
            {
                if (HasBorder())
                {
                    var border = GetOrCreateBorder();
                    return border.BorderThickness;
                }
                else
                {
                    return default(Thickness);
                }
            }
            set
            {
                var border = GetOrCreateBorder();
                border.BorderThickness = value;

                if (border.BorderBrush == null)
                {
                    var brush = GetOrCreateBorderProps().Brush;
                    BorderBrush = brush;
                }
            }
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public void AddView(DependencyObject child, int index)
        {
            if (HasBorder())
            {
                index++;
            }

            var uiElementChild = child.As<UIElement>();
            Children.Insert(index, uiElementChild);
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public DependencyObject GetChildAt(int index)
        {
            if (HasBorder())
            {
                index++;
            }

            return Children[index];
        }

        /// <summary>
        /// Gets the number of children.
        /// </summary>
        /// <returns>The number of children.</returns>
        public int GetChildCount()
        {
            var count = Children.Count;

            if (HasBorder())
            {
                count--;
            }

            return count;
        }

        /// <summary>
        /// Removes all children.
        /// </summary>
        public void RemoveAllChildren()
        {
            if (HasBorder())
            {
                for (var i = Children.Count - 1; i > 0; i--)
                {
                    Children.RemoveAt(i);
                }
            }
            else
            {
                Children.Clear();
            }
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="index">The index.</param>
        public void RemoveChildAt(int index)
        {
            if (HasBorder())
            {
                index++;
            }

            Children.RemoveAt(index);
        }

#if WINDOWS_UWP
        /// <inheritdoc />
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            return new DynamicAutomationPeer<BorderedCanvas>(this);
        }

        // TODO: implement runtime change raising event to screen reader #1562
        /// <inheritdoc />
        public AccessibilityTrait[] AccessibilityTraits { get; set; }
#endif
    }
}
