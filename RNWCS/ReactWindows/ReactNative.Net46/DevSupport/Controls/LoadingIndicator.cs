// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Windows;
using System.Windows.Controls;

namespace ReactNative.DevSupport.Controls
{
    /// <summary>
    /// A control featuring a range of loading indicating animations.
    /// </summary>
    [TemplatePart(Name = "Border", Type = typeof(Border))]
    public class LoadingIndicator : Control
    {
        /// <summary>
        /// Property used to set the speed of the animation...
        /// </summary>
        public static readonly DependencyProperty SpeedRatioProperty =
            DependencyProperty.Register("SpeedRatio", typeof(double), typeof(LoadingIndicator), new PropertyMetadata(1d, (o, e) =>
            {
                var li = (LoadingIndicator)o;

                if (li.PART_Border == null || li.IsActive == false)
                {
                    return;
                }

                foreach (VisualStateGroup group in VisualStateManager.GetVisualStateGroups(li.PART_Border))
                {
                    if (group.Name == "ActiveStates")
                    {
                        foreach (VisualState state in group.States)
                        {
                            if (state.Name == "Active")
                            {
                                state.Storyboard.SetSpeedRatio(li.PART_Border, (double)e.NewValue);
                            }
                        }
                    }
                }
            }));

        /// <summary>
        /// Property used to indicate whether the animation should be made active...
        /// </summary>
        public static readonly DependencyProperty IsActiveProperty =
            DependencyProperty.Register("IsActive", typeof(bool), typeof(LoadingIndicator), new PropertyMetadata(true, (o, e) =>
            {
                var li = (LoadingIndicator)o;

                if (li.PART_Border == null)
                {
                    return;
                }

                if ((bool)e.NewValue == false)
                {
                    VisualStateManager.GoToElementState(li.PART_Border, "Inactive", false);
                    li.PART_Border.Visibility = Visibility.Collapsed;
                }
                else
                {
                    VisualStateManager.GoToElementState(li.PART_Border, "Active", false);
                    li.PART_Border.Visibility = Visibility.Visible;

                    foreach (VisualStateGroup group in VisualStateManager.GetVisualStateGroups(li.PART_Border))
                    {
                        if (group.Name == "ActiveStates")
                        {
                            foreach (VisualState state in group.States)
                            {
                                if (state.Name == "Active")
                                {
                                    state.Storyboard.SetSpeedRatio(li.PART_Border, li.SpeedRatio);
                                }
                            }
                        }
                    }
                }
            }));

        /// <summary>
        /// Border Template Part Accessor
        /// </summary>
        protected Border PART_Border;

        /// <summary>
        /// Get/set the speed ratio of the animation.
        /// </summary>
        public double SpeedRatio
        {
            get { return (double)GetValue(SpeedRatioProperty); }
            set { SetValue(SpeedRatioProperty, value); }
        }

        /// <summary>
        /// Get/set whether the loading indicator is active.
        /// </summary>
        public bool IsActive
        {
            get { return (bool)GetValue(IsActiveProperty); }
            set { SetValue(IsActiveProperty, value); }
        }

        /// <summary>
        /// When overridden in a derived class, is invoked whenever application code
        /// or internal processes call System.Windows.FrameworkElement.ApplyTemplate().
        /// </summary>
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            PART_Border = (Border)GetTemplateChild("PART_Border");

            if (PART_Border != null)
            {
                VisualStateManager.GoToElementState(PART_Border, (this.IsActive ? "Active" : "Inactive"), false);
                foreach (VisualStateGroup group in VisualStateManager.GetVisualStateGroups(PART_Border))
                {
                    if (group.Name == "ActiveStates")
                    {
                        foreach (VisualState state in group.States)
                        {
                            if (state.Name == "Active")
                            {
                                state.Storyboard.SetSpeedRatio(PART_Border, this.SpeedRatio);
                            }
                        }
                    }
                }

                PART_Border.Visibility = (this.IsActive ? Visibility.Visible : Visibility.Collapsed);
            }
        }

        /// <summary>
        /// Initializes a new instance of the class.
        /// </summary>
        public LoadingIndicator()
        {
        }
    }
}
