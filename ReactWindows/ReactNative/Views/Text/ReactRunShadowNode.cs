using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Documents;

namespace ReactNative.Views.Text
{
    /// <summary>
    /// The shadow node class for raw text.
    /// </summary>
    public class ReactRunShadowNode : ReactInlineShadowNode
    {
        private string _text;

        /// <summary>
        /// Sets the raw text.
        /// </summary>
        /// <param name="text">The text.</param>
        [ReactProp("text")]
        public void SetText(string text)
        {
            _text = text ?? "";
            MarkUpdated();
        }

        /// <summary>
        /// Create the <see cref="Inline"/> instance for the measurement calculation.
        /// </summary>
        /// <param name="children">The children.</param>
        /// <returns>The instance.</returns>
        public override Inline MakeInline(IList<Inline> children)
        {
            if (children.Count > 0)
            {
                throw new InvalidOperationException("Raw text nodes must be leaf nodes.");
            }

            var run = new Run();
            UpdateInline(run);
            return run;
        }

        /// <summary>
        /// Update the properties on the inline instance.
        /// </summary>
        /// <param name="inline">The instance.</param>
        public override void UpdateInline(Inline inline)
        {
            ((Run)inline).Text = _text;
        }
    }
}
