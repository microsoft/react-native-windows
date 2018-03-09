namespace ReactNative.UIManager.LayoutAnimation
{
    /// <summary>
    /// Layout animation for deleted views.
    /// </summary>
    class LayoutDeleteAnimation : BaseLayoutAnimation
    {
        /// <summary>
        /// Signals if the animation should be performed in reverse.
        /// </summary>
        protected override bool IsReverse
        {
            get
            {
                return true;
            }
        }
    }
}
