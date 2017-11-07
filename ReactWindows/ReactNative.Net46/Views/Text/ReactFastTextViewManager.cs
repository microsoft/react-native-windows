// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Windows.Controls;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The view manager for simple text views.
    /// </summary>
    public class ReactSimpleTextViewManager : ReactTextViewManager
    {
        /// <summary>
        /// Name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTSimpleText";
            }
        }

        /// <summary>
        /// Gets the number of children for the view.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        /// <returns>The number of children.</returns>
        public override int GetChildCount(TextBlock parent)
        {
            return 0;
        }

        /// <summary>
        /// Removes the children from the view.
        /// </summary>
        /// <param name="parent">The view parent.</param>
        public override void RemoveAllChildren(TextBlock parent)
        {
        }
    }
}
