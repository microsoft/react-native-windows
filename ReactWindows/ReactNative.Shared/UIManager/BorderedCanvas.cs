// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
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
        private static readonly ThreadLocal<Brush> s_defaultBorderBrush = new ThreadLocal<Brush>(() => new SolidColorBrush(Colors.Black));

        private static readonly Brush s_defaultBackgroundBrush
#if WINDOWS_UWP
            = null;
#else
            = new SolidColorBrush(Color.FromArgb(0, 0, 0, 0));
#endif

        private static Brush DefaultBorderBrush => s_defaultBorderBrush.Value;

        private readonly ReactUIElementCollection _children;

        private Border _border = null;
        private Brush _borderBrush = null;

        /// <summary>
        /// Instantiates the <see cref="BorderedCanvas"/>.
        /// </summary>
        public BorderedCanvas()
        {
            _children = new ReactUIElementCollection(base.Children);
        }

        /// <summary>
        /// The collection of child elements.
        /// </summary>
        public new IList<UIElement> Children => _children;

        /// <summary>
        /// The background brush.
        /// </summary>
        /// <remarks>
        /// Do not use Canvas.Background or Border.Background directly.
        /// </remarks>
        public new Brush Background
        {
            get => _border == null ? base.Background : _border.Background;
            set
            {
                if (_border == null)
                {
                    base.Background = value ?? s_defaultBackgroundBrush;
                }
                else
                {
                    _border.Background = value;
                }
            }
        }

        /// <summary>
        /// The corner radius.
        /// </summary>
        public CornerRadius CornerRadius
        {
            get => _border != null ? _border.CornerRadius : default(CornerRadius);
            set => GetOrCreateBorder().CornerRadius = value;
        }

        /// <summary>
        /// The border brush.
        /// </summary>
        public Brush BorderBrush
        {
            get => _border != null ? _border.BorderBrush : _borderBrush;
            set
            {
                if (_border == null)
                {
                    _borderBrush = value;
                }
                else
                {
                    GetOrCreateBorder().BorderBrush = value ?? DefaultBorderBrush;
                }
            }
        }

        /// <summary>
        /// The border thickness.
        /// </summary>
        public Thickness BorderThickness
        {
            get => _border != null ? _border.BorderThickness : default(Thickness);
            set => GetOrCreateBorder().BorderThickness = value;
        }
#if WINDOWS_UWP

        // TODO: implement runtime change raising event to screen reader #1562
        /// <inheritdoc />
        public AccessibilityTrait[] AccessibilityTraits { get; set; }

        /// <inheritdoc />
        protected override AutomationPeer OnCreateAutomationPeer()
        {
            return new DynamicAutomationPeer<BorderedCanvas>(this);
        }
#endif

        private Border GetOrCreateBorder()
        {
            if (_border == null)
            {
                // Create border with current or default brush
                _border = new Border
                {
                    BorderBrush = _borderBrush ?? DefaultBorderBrush
                };

                // Bind border width and height to canvas dimensions
                _border.SetBinding(WidthProperty, new Binding
                {
                    Source = this,
                    Path = new PropertyPath("Width")
                });

                _border.SetBinding(HeightProperty, new Binding
                {
                    Source = this,
                    Path = new PropertyPath("Height")
                });

                // Transfer background to border
                if (base.Background != null && base.Background != s_defaultBackgroundBrush)
                {
                    _border.Background = base.Background;
                    base.Background = s_defaultBackgroundBrush;
                }

                // Add border to children
                _children.Add(_border);
            }

            return _border;
        }

        class ReactUIElementCollection : IList<UIElement>
        {
            private readonly UIElementCollection _inner;
            private int _offset;

            public ReactUIElementCollection(UIElementCollection innerCollection)
            {
                _inner = innerCollection;
            }

            #region Border and native children management

            public void Add(UIElement element)
            {
                _inner.Insert(_offset++, element);
            }

            #endregion

            #region React children management

            UIElement IList<UIElement>.this[int index]
            {
                get => _inner[index + _offset];
                set => _inner[index + _offset] = value;
            }

            int ICollection<UIElement>.Count => _inner.Count - _offset;

            bool ICollection<UIElement>.IsReadOnly => false;

            void ICollection<UIElement>.Add(UIElement item) => _inner.Add(item);

            bool ICollection<UIElement>.Contains(UIElement item) => IndexOf(item) >= 0;

            void ICollection<UIElement>.CopyTo(UIElement[] array, int arrayIndex)
            {
                if (array == null)
                    throw new ArgumentNullException(nameof(array));
                if (arrayIndex < 0)
                    throw new ArgumentOutOfRangeException(nameof(arrayIndex), "The argument must be greater than or equal to zero.");

                if ((array.Length - arrayIndex) < (_inner.Count - _offset))
                {
                    throw new ArgumentException("Destination array was not long enough.", nameof(array));
                }

                for (var i = 0; i < _inner.Count - _offset; ++i)
                {
                    array[arrayIndex + i] = _inner[i + _offset];
                }
            }

            int IList<UIElement>.IndexOf(UIElement item) => IndexOf(item);

            void IList<UIElement>.Insert(int index, UIElement item)
            {
                if (index < 0)
                    throw new ArgumentOutOfRangeException(nameof(index), "The argument must be greater than or equal to zero.");

                _inner.Insert(index + _offset, item);
            }

            bool ICollection<UIElement>.Remove(UIElement item)
            {
                var index = IndexOf(item);

                if (index >= 0)
                {
                    _inner.RemoveAt(index);
                    return true;
                }

                return false;
            }

            void IList<UIElement>.RemoveAt(int index)
            {
                if (index < 0)
                    throw new ArgumentOutOfRangeException(nameof(index), "The argument must be greater than or equal to zero.");

                _inner.RemoveAt(index + _offset);
            }

            void ICollection<UIElement>.Clear()
            {
                for (var i = _offset; i < _inner.Count; ++i)
                {
                    _inner.RemoveAt(i);
                }
            }

            IEnumerator<UIElement> IEnumerable<UIElement>.GetEnumerator()
            {
                return EnumerateAfterOffset().GetEnumerator();
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return EnumerateAfterOffset().GetEnumerator();
            }

            private int IndexOf(UIElement item)
            {
                for (var i = _offset; i < _inner.Count; ++i)
                {
                    if (_inner[i] == item)
                    {
                        return i - _offset;
                    }
                }

                return -1;
            }

            private IEnumerable<UIElement> EnumerateAfterOffset()
            {
                for (var i = _offset; i < _inner.Count; ++i)
                {
                    yield return _inner[i];
                }
            }

            #endregion
        }
    }
}
