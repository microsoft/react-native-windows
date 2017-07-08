namespace Extensions
{
    /// <summary>
    /// Generic implementation of an Array
    /// </summary>
    /// <typeparam name="T">The type of array</typeparam>
    public static class Array<T>
    {
        /// <summary>
        /// Create an empty array of specified type
        /// </summary>
        public static readonly T[] Empty = new T[0];
    }
}
