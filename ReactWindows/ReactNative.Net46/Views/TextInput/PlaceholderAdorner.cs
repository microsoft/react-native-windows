// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Documents;
using System.Windows.Media;

namespace ReactNative.Views.TextInput
{
    /// <summary>
    ///   Represents an adorner that adds placeholder text to a <see cref="T:System.Windows.Controls.TextBox"/>,
    ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
    /// </summary>
    public class PlaceholderAdorner : Adorner
    {
        #region Private Fields

        /// <summary>
        ///   <see langword="true"/> when the placeholder text is visible, <see langword="false" /> otherwise.
        ///   Used to avoid calling <see cref="M:System.Windows.UIElement.InvalidateVisual"/> unnecessarily.
        /// </summary>
        private bool _isPlaceholderVisible;

        #endregion

        #region Attached Properties

        #region Text Property

        /// <summary>
        ///   Gets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.Text attached property for a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element from which the property value is read.
        /// </param>
        /// <returns>
        ///   The placeholder text property value for the element.
        /// </returns>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public static string GetText(Control adornedElement)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            return (string)adornedElement.GetValue(TextProperty);
        }

        /// <summary>
        ///   Sets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.Text attached property to a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to which the attached property is written.
        /// </param>
        /// <param name="placeholderText">
        ///   The needed placeholder text value.
        /// </param>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        /// <exception cref="T:InvalidOperationException">
        ///   Raised when adornedElement is not a <see cref="T:System.Windows.Controls.TextBox"/>,
        ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
        /// </exception>
        public static void SetText(Control adornedElement, string placeholderText)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            adornedElement.SetValue(TextProperty, placeholderText);
        }

        /// <summary>
        ///   Identifies the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.Text attached property.
        /// </summary>
        public static readonly DependencyProperty TextProperty = DependencyProperty.RegisterAttached(
            "Text",
            typeof(string),
            typeof(PlaceholderAdorner),
            new FrameworkPropertyMetadata(string.Empty, OnTextChanged));

        /// <summary>
        ///   Invoked whenever <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.Text attached property is changed.
        /// </summary>
        /// <param name="sender">
        ///   The object where the event handler is attached.
        /// </param>
        /// <param name="e">
        ///   Provides data about the event.
        /// </param>
        private static void OnTextChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            if (((string)e.OldValue).Equals((string)e.NewValue, StringComparison.Ordinal))
                return;

            var adornedElement = sender as Control;

            if (adornedElement == null) return;

            if (adornedElement.IsLoaded)
            {
                AddAdorner(adornedElement);
            }
            else
            {
                adornedElement.Loaded += AdornedElement_Loaded;
            }
        }

        #endregion

        #region TextColor Property

        /// <summary>
        ///   Gets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.TextColor attached property for a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element from which the property value is read.
        /// </param>
        /// <returns>
        ///   The placeholder text color property value for the element.
        /// </returns>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public static Brush GetTextColor(Control adornedElement)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            return (Brush)adornedElement.GetValue(TextColorProperty);
        }

        /// <summary>
        ///   Sets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.TextColor attached property to a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to which the attached property is written.
        /// </param>
        /// <param name="placeholderTextColor">
        ///   The needed placeholder text color brush value.
        /// </param>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        /// <exception cref="T:InvalidOperationException">
        ///   Raised when adornedElement is not a <see cref="T:System.Windows.Controls.TextBox"/>,
        ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
        /// </exception>
        public static void SetTextColor(Control adornedElement, Brush placeholderTextColor)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            adornedElement.SetValue(TextColorProperty, placeholderTextColor);
        }

        /// <summary>
        ///   Identifies the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.TextColor attached property.
        /// </summary>
        public static readonly DependencyProperty TextColorProperty =
            DependencyProperty.RegisterAttached("TextColor", typeof(Brush), typeof(PlaceholderAdorner),
                new FrameworkPropertyMetadata(null));

        #endregion

        #region FontFamily Property

        /// <summary>
        ///   Gets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontFamily attached property for a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element from which the property value is read.
        /// </param>
        /// <returns>
        ///   The placeholder text font family property value for the element.
        /// </returns>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public static FontFamily GetFontFamily(Control adornedElement)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            return (FontFamily)adornedElement.GetValue(FontFamilyProperty);
        }

        /// <summary>
        ///   Sets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontFamily attached property to a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to which the attached property is written.
        /// </param>
        /// <param name="fontFamily">
        ///   The placeholder text font family value.
        /// </param>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        /// <exception cref="T:InvalidOperationException">
        ///   Raised when adornedElement is not a <see cref="T:System.Windows.Controls.TextBox"/>,
        ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
        /// </exception>
        public static void SetFontFamily(Control adornedElement, FontFamily fontFamily)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            adornedElement.SetValue(FontFamilyProperty, fontFamily);
        }

        /// <summary>
        ///   Identifies the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontFamily attached property.
        /// </summary>
        public static readonly DependencyProperty FontFamilyProperty =
            DependencyProperty.RegisterAttached("FontFamily", typeof(FontFamily), typeof(PlaceholderAdorner), new PropertyMetadata(null));

        #endregion

        #region FontStyle Property

        /// <summary>
        ///   Gets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontStyle attached property for a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element from which the property value is read.
        /// </param>
        /// <returns>
        ///   The placeholder text font style property value for the element.
        /// </returns>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public static FontStyle? GetFontStyle(Control adornedElement)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            return (FontStyle?)adornedElement.GetValue(FontStyleProperty);
        }

        /// <summary>
        ///   Sets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontStyle attached property to a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to which the attached property is written.
        /// </param>
        /// <param name="fontStyle">
        ///   The placeholder text Font style value.
        /// </param>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        /// <exception cref="T:InvalidOperationException">
        ///   Raised when adornedElement is not a <see cref="T:System.Windows.Controls.TextBox"/>,
        ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
        /// </exception>
        public static void SetFontStyle(Control adornedElement, FontStyle? fontStyle)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            adornedElement.SetValue(FontStyleProperty, fontStyle);
        }

        /// <summary>
        ///   Identifies the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontStyle attached property.
        /// </summary>
        public static readonly DependencyProperty FontStyleProperty =
            DependencyProperty.RegisterAttached("FontStyle", typeof(FontStyle?), typeof(PlaceholderAdorner),
                new FrameworkPropertyMetadata(null));

        #endregion

        #region FontWeight Property

        /// <summary>
        ///   Gets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontWeight attached property for a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element from which the property value is read.
        /// </param>
        /// <returns>
        ///   The placeholder text font weight property value for the element.
        /// </returns>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public static FontWeight? GetFontWeight(Control adornedElement)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            return (FontWeight?)adornedElement.GetValue(FontWeightProperty);
        }

        /// <summary>
        ///   Sets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontWeight attached property to a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to which the attached property is written.
        /// </param>
        /// <param name="fontWeight">
        ///   The placeholder text Font weight value.
        /// </param>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        /// <exception cref="T:InvalidOperationException">
        ///   Raised when adornedElement is not a <see cref="T:System.Windows.Controls.TextBox"/>,
        ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
        /// </exception>
        public static void SetFontWeight(Control adornedElement, FontWeight? fontWeight)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            adornedElement.SetValue(FontWeightProperty, fontWeight);
        }

        /// <summary>
        ///   Identifies the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontWeight attached property.
        /// </summary>
        public static readonly DependencyProperty FontWeightProperty =
            DependencyProperty.RegisterAttached("FontWeight", typeof(FontWeight?), typeof(PlaceholderAdorner),
                new FrameworkPropertyMetadata(null));

        #endregion

        #region FontStretch Property

        /// <summary>
        ///   Gets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontStretch attached property for a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element from which the property value is read.
        /// </param>
        /// <returns>
        ///   The placeholder text font stretch property value for the element.
        /// </returns>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public static FontStretch? GetFontStretch(Control adornedElement)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            return (FontStretch?)adornedElement.GetValue(FontStretchProperty);
        }

        /// <summary>
        ///   Sets the value of the <see cref="T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontStretch attached property to a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to which the attached property is written.
        /// </param>
        /// <param name="fontStretch">
        ///   The placeholder text Font stretch value.
        /// </param>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        /// <exception cref="T:InvalidOperationException">
        ///   Raised when adornedElement is not a <see cref="T:System.Windows.Controls.TextBox"/>,
        ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
        /// </exception>
        public static void SetFontStretch(Control adornedElement, FontStretch? fontStretch)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            adornedElement.SetValue(FontStretchProperty, fontStretch);
        }

        /// <summary>
        ///   Identifies the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.FontStretch attached property.
        /// </summary>
        public static readonly DependencyProperty FontStretchProperty =
            DependencyProperty.RegisterAttached("FontStretch", typeof(FontStretch?), typeof(PlaceholderAdorner), new PropertyMetadata(null));

        #endregion

        #region HideOnFocus Property

        /// <summary>
        ///   Gets the value of the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.HideOnFocus attached property for a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element from which the property value is read.
        /// </param>
        /// <returns>
        ///   A value indicating whether the control will be hidden when the element is in focus.
        /// </returns>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public static bool GetHideOnFocus(Control adornedElement)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            return (bool)adornedElement.GetValue(HideOnFocusProperty);
        }

        /// <summary>
        ///   Sets the value of the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.HideOnFocus attached property to a specified element.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to which the attached property is written.
        /// </param>
        /// <param name="hideOnFocus">
        ///   A value indicating whether to hide the placeholder text when the element is in focus.
        /// </param>
        /// <exception cref="T:ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        /// <exception cref="T:InvalidOperationException">
        ///   Raised when adornedElement is not a <see cref="T:System.Windows.Controls.TextBox"/>,
        ///   <see cref="T:System.Windows.Controls.RichTextBox"/> or <see cref="T:System.Windows.Controls.PasswordBox"/>.
        /// </exception>
        public static void SetHideOnFocus(Control adornedElement, bool hideOnFocus)
        {
            if (adornedElement == null)
            {
                throw new ArgumentNullException(nameof(adornedElement));
            }

            adornedElement.SetValue(HideOnFocusProperty, hideOnFocus);
        }

        /// <summary>
        ///   Identifies the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.HideOnFocus attached property.
        /// </summary>
        public static readonly DependencyProperty HideOnFocusProperty = DependencyProperty.RegisterAttached(
            "HideOnFocus",
            typeof(bool),
            typeof(PlaceholderAdorner),
            new FrameworkPropertyMetadata(false, OnHideOnFocusChanged));

        /// <summary>
        ///   Invoked whenever <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner" />.HideOnFocus attached property is changed.
        /// </summary>
        /// <param name="sender">
        ///   The object where the event handler is attached.
        /// </param>
        /// <param name="e">
        ///   Provides data about the event.
        /// </param>
        private static void OnHideOnFocusChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            bool hideOnFocus = (bool)e.NewValue;

            if ((bool)e.OldValue == hideOnFocus)
            {
                return;
            }

            var adornedElement = sender as Control;

            if (!(adornedElement is TextBox || adornedElement is PasswordBox || adornedElement is RichTextBox))
            {
                if (!adornedElement.IsLoaded)
                {
                    adornedElement.Loaded += ChangeHideOnFocus;
                }
                else
                {
                    var templateTextBox = FindTextBox(adornedElement);

                    if (templateTextBox != null)
                    {
                        PlaceholderAdorner.SetHideOnFocus(templateTextBox, (bool)e.NewValue);
                    }
                }

                return;
            }

            PlaceholderAdorner placeholder = null;

            var adornerLayer = AdornerLayer.GetAdornerLayer(adornedElement);

            if (adornerLayer == null)
            {
                return;
            }

            var adorners = adornerLayer.GetAdorners(adornedElement);

            if (adorners != null)
            {
                foreach (Adorner adorner in adorners)
                {
                    placeholder = adorner as PlaceholderAdorner;

                    if (placeholder != null)
                    {
                        break;
                    }
                }
            }

            if (placeholder == null)
            {
                return;
            }

            if (adornedElement.IsLoaded)
            {
                if (hideOnFocus)
                {
                    adornedElement.GotFocus += placeholder.AdornedElement_GotFocus;
                    adornedElement.LostFocus += placeholder.AdornedElement_LostFocus;

                    if (adornedElement.IsFocused && placeholder._isPlaceholderVisible)
                    {
                        placeholder.InvalidateVisual();
                    }
                }
                else
                {
                    adornedElement.GotFocus -= placeholder.AdornedElement_GotFocus;
                    adornedElement.LostFocus -= placeholder.AdornedElement_LostFocus;

                    placeholder.AdornedElement_LostFocus(adornedElement,
                        new RoutedEventArgs(UIElement.LostFocusEvent, placeholder));
                }
            }
            else
            {
                adornedElement.Loaded += AdornedElement_Loaded;
            }

        }

        #endregion

        #endregion

        #region Constructors
        /// <summary>
        ///   Initializes static members of the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner"/> class.
        /// </summary>
        static PlaceholderAdorner()
        {
            IsHitTestVisibleProperty.OverrideMetadata(typeof(PlaceholderAdorner), new FrameworkPropertyMetadata(false));
            ClipToBoundsProperty.OverrideMetadata(typeof(PlaceholderAdorner), new FrameworkPropertyMetadata(true));
        }

        /// <summary>
        ///   Initializes a new instance of the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner"/> class.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to bind the adorner to.
        /// </param>
        /// <exception cref="T:System.ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public PlaceholderAdorner(PasswordBox adornedElement)
            : this((Control)adornedElement)
        {
            if (!(adornedElement.IsFocused && (bool)adornedElement.GetValue(HideOnFocusProperty)))
            {
                adornedElement.PasswordChanged += this.AdornedElement_ContentChanged;
            }
        }

        /// <summary>
        ///   Initializes a new instance of the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner"/> class.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to bind the adorner to.
        /// </param>
        /// <exception cref="T:System.ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        public PlaceholderAdorner(TextBoxBase adornedElement)
            : this((Control)adornedElement)
        {

            //if ((bool) adornedElement.GetValue(ShowWhenContentEmptyProperty))
            //{
            //    adornedElement.TextChanged += this.AdornedElement_ContentChanged;
            //}

            if (!(adornedElement.IsFocused && (bool)adornedElement.GetValue(HideOnFocusProperty)))
            {
                adornedElement.TextChanged += this.AdornedElement_ContentChanged;
            }
        }

        /// <summary>
        ///   Initializes a new instance of the <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner"/> class.
        /// </summary>
        /// <param name="adornedElement">
        ///   The element to bind the adorner to.
        /// </param>
        /// <exception cref="T:System.ArgumentNullException">
        ///   Raised when adornedElement is null.
        /// </exception>
        protected PlaceholderAdorner(Control adornedElement)
            : base(adornedElement)
        {
            if ((bool)adornedElement.GetValue(HideOnFocusProperty))
            {
                adornedElement.GotFocus += this.AdornedElement_GotFocus;
                adornedElement.LostFocus += this.AdornedElement_LostFocus;
            }
        }
        #endregion

        #region Override Methods

        /// <summary>
        ///   Draws the content of a <see cref="T:System.Windows.Media.DrawingContext" /> object during the render pass of a <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner"/> element.
        /// </summary>
        /// <param name="drawingContext">
        ///   The <see cref="T:System.Windows.Media.DrawingContext" /> object to draw. This context is provided to the layout system.
        /// </param>
        protected override void OnRender(DrawingContext drawingContext)
        {
            var adornedElement = this.AdornedElement as Control;

            string placeholderText;

            var hideOnFocus = (bool?)adornedElement?.GetValue(HideOnFocusProperty) ?? true;

            if (adornedElement == null || (adornedElement.IsFocused && hideOnFocus) || !this.IsElementEmpty() || string.IsNullOrEmpty(placeholderText = (string)adornedElement.GetValue(TextProperty)))
            {
                this._isPlaceholderVisible = false;

                return;
            }

            this._isPlaceholderVisible = true;

            var size = adornedElement.RenderSize;

            var maxHeight = size.Height - adornedElement.BorderThickness.Top -
                                adornedElement.BorderThickness.Bottom - adornedElement.Padding.Top -
                                adornedElement.Padding.Bottom;

            var maxWidth = size.Width - adornedElement.BorderThickness.Left -
                                adornedElement.BorderThickness.Right - adornedElement.Padding.Left -
                                adornedElement.Padding.Right - 4.0;

            if (maxHeight <= 0 || maxWidth <= 0)
            {
                return;
            }

            var computedTextAlignment = this.ComputedTextAlignment();

            // Foreground brush does not need to be dynamic. OnRender called when SystemColors changes.
            var foreground = (Brush)adornedElement.GetValue(TextColorProperty) ?? adornedElement.Foreground;

            var fontFamily = (FontFamily)adornedElement.GetValue(FontFamilyProperty) ?? adornedElement.FontFamily;

            var fontStyle = (FontStyle?)adornedElement.GetValue(FontStyleProperty) ?? adornedElement.FontStyle;

            var fontWeight = (FontWeight?)adornedElement.GetValue(FontWeightProperty) ?? adornedElement.FontWeight;

            var fontStretch = (FontStretch?)adornedElement.GetValue(FontStretchProperty) ?? adornedElement.FontStretch;

            var typeface = new Typeface(fontFamily, fontStyle, fontWeight, fontStretch);

            var formattedText = new FormattedText(placeholderText, CultureInfo.CurrentCulture, adornedElement.FlowDirection, typeface, adornedElement.FontSize, foreground)
            {
                TextAlignment = computedTextAlignment,
                MaxTextHeight = maxHeight,
                MaxTextWidth = maxWidth
            };

            double left;
            double top = 0.0;
            if (adornedElement.FlowDirection == FlowDirection.RightToLeft)
            {
                left = adornedElement.BorderThickness.Right + adornedElement.Padding.Right + 2.0;
            }
            else
            {
                left = adornedElement.BorderThickness.Left + adornedElement.Padding.Left + 2.0;
            }

            switch (adornedElement.VerticalContentAlignment)
            {
                case VerticalAlignment.Top:
                case VerticalAlignment.Stretch:
                    top = adornedElement.BorderThickness.Top + adornedElement.Padding.Top;
                    break;
                case VerticalAlignment.Bottom:
                    top = size.Height - adornedElement.BorderThickness.Bottom - adornedElement.Padding.Bottom - formattedText.Height;
                    break;
                case VerticalAlignment.Center:
                    top = (size.Height + adornedElement.BorderThickness.Top - adornedElement.BorderThickness.Bottom +
                            adornedElement.Padding.Top - adornedElement.Padding.Bottom - formattedText.Height) / 2.0;
                    break;
            }

            if (adornedElement.FlowDirection == FlowDirection.RightToLeft)
            {
                // Somehow everything got drawn reflected. Add a transform to correct.
                drawingContext.PushTransform(new ScaleTransform(-1.0, 1.0, RenderSize.Width / 2.0, 0.0));
                drawingContext.DrawText(formattedText, new Point(left, top));
                drawingContext.Pop();
            }
            else
            {
                drawingContext.DrawText(formattedText, new Point(left, top));
            }
        }

        #endregion

        #region Event Handlers
        /// <summary>
        ///   Event handler for AdornedElement.GotFocus.
        /// </summary>
        /// <param name="sender">
        ///   The AdornedElement where the event handler is attached.
        /// </param>
        /// <param name="e">
        ///   Provides data about the event.
        /// </param>
        private void AdornedElement_GotFocus(object sender, RoutedEventArgs e)
        {
            var textBoxBase = AdornedElement as TextBoxBase;

            if (textBoxBase != null)
            {
                textBoxBase.TextChanged -= this.AdornedElement_ContentChanged;
            }
            else
            {
                var passwordBox = AdornedElement as PasswordBox;

                if (passwordBox != null)
                {
                    passwordBox.PasswordChanged -= this.AdornedElement_ContentChanged;
                }
            }

            if (this._isPlaceholderVisible)
            {
                this.InvalidateVisual();
            }
        }

        /// <summary>
        ///   Event handler for AdornedElement.LostFocus.
        /// </summary>
        /// <param name="sender">
        ///   The AdornedElement where the event handler is attached.
        /// </param>
        /// <param name="e">
        ///   Provides data about the event.
        /// </param>
        private void AdornedElement_LostFocus(object sender, RoutedEventArgs e)
        {
            var textBoxBase = AdornedElement as TextBoxBase;

            if (textBoxBase != null)
            {
                textBoxBase.TextChanged += this.AdornedElement_ContentChanged;
            }
            else
            {
                var passwordBox = AdornedElement as PasswordBox;

                if (passwordBox != null)
                {
                    passwordBox.PasswordChanged += this.AdornedElement_ContentChanged;
                }
            }

            if (!this._isPlaceholderVisible && this.IsElementEmpty())
            {
                this.InvalidateVisual();
            }
        }

        /// <summary>
        ///   Event handler for AdornedElement.ContentChanged.
        /// </summary>
        /// <param name="sender">
        ///   The AdornedElement where the event handler is attached.
        /// </param>
        /// <param name="e">
        ///   Provides data about the event.
        /// </param>
        private void AdornedElement_ContentChanged(object sender, RoutedEventArgs e)
        {
            var adornedElement = sender as Control;


            if (this._isPlaceholderVisible ^ this.IsElementEmpty())
            {
                this.InvalidateVisual();
            }
        }

        #endregion

        #region Private Static Methods

        /// <summary>
        ///   Adds a <see cref="T:T:ReactNative.Views.TextInput.PlaceholderAdorner"/> to the adorner layer.
        /// </summary>
        /// <param name="adornedElement">
        ///   The adorned element.
        /// </param>
        private static void AddAdorner(Control adornedElement)
        {
            var adornerLayer = AdornerLayer.GetAdornerLayer(adornedElement);

            if (adornerLayer == null)
            {
                return;
            }

            var adorners = adornerLayer.GetAdorners(adornedElement);
            if (adorners != null)
            {
                foreach (var adorner in adorners)
                {
                    if (adorner is PlaceholderAdorner)
                    {
                        adorner.InvalidateVisual();
                        return;
                    }
                }
            }

            var textBox = adornedElement as TextBox;

            if (textBox != null)
            {
                adornerLayer.Add(new PlaceholderAdorner(textBox));
                return;
            }

            var richTextBox = adornedElement as RichTextBox;

            if (richTextBox != null)
            {
                adornerLayer.Add(new PlaceholderAdorner(richTextBox));
                return;
            }

            var passwordBox = adornedElement as PasswordBox;

            if (passwordBox != null)
            {
                adornerLayer.Add(new PlaceholderAdorner(passwordBox));
                return;
            }

            // TextBox is hidden in template. Search for it.
            TextBox templateTextBox = null;
            templateTextBox = FindTextBox(adornedElement);
            if (templateTextBox != null)
            {
                PlaceholderAdorner.SetText(templateTextBox, (string)PlaceholderAdorner.GetText(adornedElement));
            }
        }

        /// <summary>
        ///   Finds a <see cref="T:System.Windows.Controls.TextBox"/> in the visual tree of the adorned element using a breadth first search.
        /// </summary>
        /// <param name="adornedElement">The adorned element which visual tree is searched for a <see cref="T:System.Windows.Controls.TextBox"/>.</param>
        /// <returns>The <see cref="T:System.Windows.Controls.TextBox"/> if one is found, <see langword="null"/> if none exists.</returns>
        private static TextBox FindTextBox(Control adornedElement)
        {
            TextBox templateTextBox = null;

            Queue<DependencyObject> queue = new Queue<DependencyObject>();
            queue.Enqueue(adornedElement);

            while (queue.Count > 0)
            {
                var element = queue.Dequeue();

                templateTextBox = element as TextBox;
                if (templateTextBox != null)
                {
                    break;
                }

                for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); ++i)
                {
                    queue.Enqueue(VisualTreeHelper.GetChild(element, i));
                }
            }

            return templateTextBox;
        }

        /// <summary>
        ///   Event handler for AdornedElement.Loaded.
        /// </summary>
        /// <param name="sender">
        ///   The AdornedElement where the event handler is attached.
        /// </param>
        /// <param name="e">
        ///   Provides data about the event.
        /// </param>
        private static void AdornedElement_Loaded(object sender, RoutedEventArgs e)
        {
            var adornedElement = (Control)sender;

            adornedElement.Loaded -= AdornedElement_Loaded;

            AddAdorner(adornedElement);
        }

        /// <summary>
        ///   Changes the HideOnFocus property of the text box in the visual tree of the sender.
        /// </summary>
        /// <param name="sender">The object where the event handler is attached.</param>
        /// <param name="e">Provides data about the event.</param>
        private static void ChangeHideOnFocus(object sender, RoutedEventArgs e)
        {
            var adornedElement = sender as Control;

            if (adornedElement == null)
            {
                return;
            }

            var templateTextBox = FindTextBox(adornedElement);

            if (templateTextBox != null)
            {
                PlaceholderAdorner.SetHideOnFocus(templateTextBox, PlaceholderAdorner.GetHideOnFocus(adornedElement));
            }
        }

        #endregion

        #region Private Methods

        /// <summary>
        ///   Checks if the content of the adorned element is empty.
        /// </summary>
        /// <returns>
        ///   Returns <see langword="true" /> if the content is empty, <see langword="false" /> otherwise.
        /// </returns>
        private bool IsElementEmpty()
        {
            var adornedElement = AdornedElement;

            var textBox = adornedElement as TextBox;

            if (textBox != null)
            {
                return string.IsNullOrEmpty(textBox.Text);
            }

            var passwordBox = adornedElement as PasswordBox;

            if (passwordBox != null)
            {
                return string.IsNullOrEmpty(passwordBox.Password);
            }

            var richTextBox = adornedElement as RichTextBox;

            if (richTextBox != null)
            {
                var blocks = richTextBox.Document.Blocks;
                if (blocks.Count == 0)
                {
                    return true;
                }

                if (blocks.Count == 1)
                {
                    var paragraph = blocks.FirstBlock as Paragraph;
                    if (paragraph == null)
                    {
                        return false;
                    }

                    if (paragraph.Inlines.Count == 0)
                    {
                        return true;
                    }

                    if (paragraph.Inlines.Count == 1)
                    {
                        var run = paragraph.Inlines.FirstInline as Run;

                        return run != null && string.IsNullOrEmpty(run.Text);
                    }
                }

                return false;
            }

            return false;
        }

        /// <summary>
        ///   Computes the text alignment of the adorned element.
        /// </summary>
        /// <returns>
        ///   Returns the computed text alignment.
        /// </returns>
        private TextAlignment ComputedTextAlignment()
        {
            var adornedElement = AdornedElement as Control;

            var textBox = adornedElement as TextBox;

            if (textBox != null)
            {
                if (DependencyPropertyHelper.GetValueSource(textBox, TextBox.HorizontalContentAlignmentProperty)
                        .BaseValueSource != BaseValueSource.Local || DependencyPropertyHelper.GetValueSource(textBox, TextBox.TextAlignmentProperty)
                        .BaseValueSource == BaseValueSource.Local)
                {
                    // TextAlignment dominates
                    return textBox.TextAlignment;
                }
            }

            var richTextBox = adornedElement as RichTextBox;

            if (richTextBox != null)
            {
                var blocks = richTextBox.Document.Blocks;

                var textAlignment = richTextBox.Document.TextAlignment;

                if (blocks.Count == 0)
                {
                    return textAlignment;
                }

                if (blocks.Count == 1)
                {
                    var paragraph = blocks.FirstBlock as Paragraph;
                    if (paragraph == null)
                    {
                        return textAlignment;
                    }

                    return paragraph.TextAlignment;
                }

                return textAlignment;
            }

            if (adornedElement != null)
            {
                switch (adornedElement.HorizontalContentAlignment)
                {
                    case HorizontalAlignment.Left:
                        return TextAlignment.Left;
                    case HorizontalAlignment.Right:
                        return TextAlignment.Right;
                    case HorizontalAlignment.Center:
                        return TextAlignment.Center;
                    case HorizontalAlignment.Stretch:
                        return TextAlignment.Justify;
                }
            }

            return TextAlignment.Left;
        }

        #endregion
    }
}