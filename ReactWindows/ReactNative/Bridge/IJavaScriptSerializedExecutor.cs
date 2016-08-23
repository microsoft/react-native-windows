namespace ReactNative.Bridge
{
    /// <summary>
    /// An interface which builds upon the <see cref="IJavaScriptExecutor"/> to provide a way
    /// to serialize scripts and run them.
    /// </summary>
    interface IJavaScriptSerializedExecutor : IJavaScriptExecutor
    {
        /// <summary>
        /// Serializes a script from a file location with source URI.
        /// </summary>
        /// <param name="sourceFile">The location of the source file to serialize.</param>
        /// <param name="destinationFile">The destination to save the serialized source.</param>
        void SerializeScript(string sourceFile, string destinationFile);

        /// <summary>
        /// Runs a serialzed script from the serialized path, the source file path and source URI.
        /// </summary>
        /// <param name="serializedPath">The path containing the serailized script.</param>
        /// <param name="sourceFilePath">The path of the source file.</param>
        /// <param name="sourceUri">The source URI.</param>
        void RunSerializedScript(string serializedPath, string sourceFilePath, string sourceUri);
    }
}
