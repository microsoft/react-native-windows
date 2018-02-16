using ReactNative.Bridge.Queue;

namespace ReactNative.Bridge
{
    interface IHasActionQueue
    {
        IActionQueue ActionQueue { get; }
    }
}
