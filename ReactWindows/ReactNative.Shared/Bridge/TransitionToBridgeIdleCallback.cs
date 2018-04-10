namespace ReactNative.Bridge
{
    /// <summary>
    /// Invoked once all pending JS calls have resolved via an onBatchComplete call in the <see cref="IReactInstance"/>,
    /// the requested <see cref="INativeModule"/> calls have also run.
    /// </summary>
    public delegate void TransitionToBridgeIdleCallback();
}
