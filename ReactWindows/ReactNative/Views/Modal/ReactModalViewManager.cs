// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.UI;
using ReactNative.UIManager;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Markup;
using Windows.UI.Xaml.Media;
using static System.FormattableString;

namespace ReactNative.Views.Modal
{
    /// <summary>
    /// View manager for React control instances.
    /// </summary>
    public class ReactModalViewManager : ViewParentManager<ContentDialog, ReactModalShadowNode>
    {
        private const string ControlTemplate = 
            @"<ControlTemplate TargetType=""ContentDialog"" xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation"" xmlns:x=""http://schemas.microsoft.com/winfx/2006/xaml"">
                <Border x:Name=""Container"">
                    <Grid x:Name=""LayoutRoot"">
                        <Border x:Name=""BackgroundElement""
                            Background=""{TemplateBinding Background}""
                            FlowDirection=""{TemplateBinding FlowDirection}""
                            BorderThickness=""{ThemeResource ContentDialogBorderWidth}""
                            BorderBrush=""{ThemeResource SystemControlForegroundAccentBrush}""
                            MaxWidth=""{TemplateBinding MaxWidth}""
                            MaxHeight=""{TemplateBinding MaxHeight}""
                            MinWidth=""{TemplateBinding MinWidth}""
                            MinHeight=""{TemplateBinding MinHeight}""
                        >
                            <ScrollViewer x:Name=""ContentScrollViewer""
                                HorizontalScrollBarVisibility=""Disabled""
                                VerticalScrollBarVisibility=""Disabled""
                                ZoomMode=""Disabled""
                                IsTabStop=""False""
                            >
                            <ContentPresenter x:Name=""Content""
                                ContentTemplate=""{TemplateBinding ContentTemplate}""
                                Content=""{TemplateBinding Content}""
                                FontSize=""{ThemeResource ControlContentThemeFontSize}""
                                FontFamily=""{ThemeResource ContentControlThemeFontFamily}""
                                Margin=""{ThemeResource ContentDialogContentMargin}""
                                Foreground=""{TemplateBinding Foreground}""
                                TextWrapping=""Wrap"" />
                            </ScrollViewer>
                        </Border>
                    </Grid>
                </Border>
            </ControlTemplate>";

        /// <summary>
        /// The name of this view manager. This will be the name used to 
        /// reference this view manager from JavaScript.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTModalHostView";
            }
        }

        /// <summary>
        /// This method should return the <see cref="ReactModalShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view. 
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactModalShadowNode CreateShadowNodeInstance()
        {
            return new ReactModalShadowNode();
        }

        /// <summary>
        /// Adds a child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="child">The child view.</param>
        /// <param name="index">The index.</param>
        public override void AddView(ContentDialog parent, DependencyObject child, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), Invariant($"{nameof(ContentDialog)} currently only supports one child."));
            }
            if (parent.Content != null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ContentDialog)} already has a child element."));
            }

            if (child is FrameworkElement element)
            {
                element.VerticalAlignment = VerticalAlignment.Top;
                element.HorizontalAlignment = HorizontalAlignment.Stretch;
                element.Height = double.NaN;
                element.Width = double.NaN;

                if (child is BorderedCanvas canvas)
                    canvas.Background = new SolidColorBrush(Colors.Transparent);
            }
            
            parent.Content = child;
        }

        /// <summary>
        /// Gets the child at the given index.
        /// </summary>
        /// <param name="parent">The parent view.</param>
        /// <param name="index">The index.</param>
        /// <returns>The child view.</returns>
        public override DependencyObject GetChildAt(ContentDialog parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), $"{nameof(ContentDialog)} currently only supports one child.");
            }

            return EnsureChild(parent);
        }

        /// <summary>
        /// Gets the number of children in the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(ContentDialog parent)
        {
            return parent.Content != null ? 1 : 0;
        }

        /// <summary>
        /// Removes all children from the view parent.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(ContentDialog parent)
        {
            parent.Content = null;
        }

        /// <summary>
        /// Removes the child at the given index.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <param name="index">The index.</param>
        public override void RemoveChildAt(ContentDialog parent, int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException(nameof(index), $"{nameof(ContentDialog)} currently only supports one child.");
            }

            EnsureChild(parent);
            RemoveAllChildren(parent);
        }

        public override void OnDropViewInstance(ThemedReactContext reactContext, ContentDialog view)
        {
            base.OnDropViewInstance(reactContext, view);

            view.Hide();
        }

        /// <summary>
        /// Creates a new view instance of type <see cref="Canvas"/>.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <returns>The view instance.</returns>
        protected override ContentDialog CreateViewInstance(ThemedReactContext reactContext)
        {
            var view = new ContentDialog();
            var style = new Style(typeof(ContentDialog));
            style.Setters.Add(new Setter(Control.TemplateProperty, XamlReader.Load(ControlTemplate)));
            view.Style = style;

            return view;
        }

        protected override void OnAfterUpdateTransaction(ContentDialog view)
        {
            base.OnAfterUpdateTransaction(view);

            view.Loaded += (sender, args) =>
            {
                ((ContentDialog)sender).ShowAsync().GetResults();
            };
        }

        private static DependencyObject EnsureChild(ContentDialog view)
        {
            var child = view.Content;
            if (child == null)
            {
                throw new InvalidOperationException(Invariant($"{nameof(ContentDialog)} does not have any children."));
            }

            var dependencyObject = child as DependencyObject;
            if (dependencyObject == null)
            {
                throw new InvalidOperationException(Invariant($"Invalid child element in {nameof(ContentDialog)}."));
            }

            return dependencyObject;
        }
    }
}
