namespace ReactNative.Bridge
{
    /// <summary>
    /// Interface for performance counter.
    /// </summary>
    public interface IPerformanceNow
    {
        /// <summary>
        /// Returns performance counter.
        /// </summary>
        /// <returns>The current performance counter value.</returns>
        long PerformanceNow();
    }
}
