using ReactNative.UIManager;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Interface that represents a block to execute on the UI thread.
    /// Exposes NativeViewHierarchyManager for third party libraries.
    /// </summary>
    public interface IUIBlock
    {
        /// <summary>
        /// Executes the block.
        /// </summary>
        /// <param name="nativeViewHierarchyManager">The native view hierarchy manager.</param>
        void Execute(NativeViewHierarchyManager nativeViewHierarchyManager);
    }
}