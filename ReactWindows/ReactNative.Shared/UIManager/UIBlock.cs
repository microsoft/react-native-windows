using ReactNative.UIManager;

namespace ReactNative.Shared.UIManager
{
    /// <summary>
    /// Interface that represents a block to execute on the UI thread.
    /// Exposes NativeViewHierarchyManager for third party libraries.
    /// </summary>
    public interface UIBlock
    {
        /// <summary>
        /// Executes the block.
        /// </summary>
        /// <param name="nativeViewHierarchyManager"></param>
        void Execute(NativeViewHierarchyManager nativeViewHierarchyManager);
    }
}