This document contains an overview of all the threading models in React Native Windows.

## Core Threading Model - Action Queues

The core threading model for React Native Windows is based on the concept of an action queue, i.e., a FIFO queue of `Action` delegates that are executed in order. Here's the interface:

```c#
    public interface IActionQueue : IDisposable
    {
        void Dispatch(Action action);
        bool IsOnThread();
    }
```

There are two methods, `Dispatch`, which queues an action, and `IsOnThread`, which can check if the current thread of execution is running in the context of the action queue. We chose [Rx.NET](http://github.com/Reactive-Extensions/Rx.NET) as the implementation for the action queue because many platforms already have [`IScheduler`](https://msdn.microsoft.com/en-us/library/system.reactive.concurrency.ischeduler(v=vs.103).aspx) implementations for Rx.NET that can ensure actions are executed on the proper thread. 

The `Dispatch` method calls `OnNext` on a `Subject<Action>` that is subscribed to an `ObserveOn` operator taking in the scheduler selected for the queue, and ultimately to an `IObserver<Action>` that invokes the action. For simplicity, we implemented `IsOnThread` to use `ThreadLocal<bool>` object that is set and unset before and after the action is invoked. I.e.,

```c#
    private ThreadLocal<bool> _isOnThread = new ThreadLocal<bool>();

    /* ... */

    _isOnThread.Value = true;
    try 
    {
        action()
    }
    finally
    {
        _isOnThread.Value = false;
    }
```

## JavaScript Action Queue

React Native Windows is initialized by default with a single-threaded action queue for interacting with [Chakra](https://github.com/Microsoft/ChakraCore) (the JavaScript runtime for React Native Windows). We could have used the .NET Task Pool for interacting with Chakra, but in that case we would have had to call `JsSetCurrentContext` before and after each action was executed on the JavaScript thread. With a single thread, we only have to call `JsSetCurrentContext` once after initialization and once before dispose of the Chakra runtime. The Rx.NET implementation detail here is the use of the [`NewThreadScheduler`](https://msdn.microsoft.com/en-us/library/system.reactive.concurrency.newthreadscheduler(v=vs.103).aspx), which creates a new thread and schedules all actions on that thread.

As a React Native Windows user, the best way to interact with the JavaScript action queue is through the `ReactContext.RunOnJavaScriptQueueThread` API.

## Native Modules Action Queue

React Native Windows uses the Rx.NET `Scheduler.Default` for the scheduler on the native modules action queue. The native modules action queue is the context in which native method on native modules are called. The Rx.NET `Scheduler.Default` uses the platforms default scheduling mechanism; in the case of UWP, this is the [`Windows.System.Threading.ThreadPool`](https://docs.microsoft.com/en-us/uwp/api/Windows.System.Threading.ThreadPool).

As a React Native Windows user, the best way to interact with the native modules action queue is through the `ReactContext.RunOnNativeModulesQueueThread` API.

### Custom Action Queue for Native Modules

In general, native methods only need to be executed in order inside the scope of a single native module. In some cases, it could be a bottleneck for all native modules to use the same action queue, especially if the native module is doing something that is computation heavy (although arguably you should be triggering your own background thread to do the work rather than hanging up the action queue in these cases). To make things easier, React Native Windows allows you to create a custom action queue per native module. Both `NativeModuleBase` and `ReactContextNativeModuleBase` provide a constructor that takes in an `IActionQueue`. The action queue is disposed by the framework after the `OnReactInstanceDisposed` lifecycle event.

```c#
public class MyNativeModule : NativeModuleBase
{
    public MyNativeModule(Action<Exception> onError)
        : base(new ActionQueue(onError))
    {
    }

    /* ... */

    public override void OnReactInstanceDispose()
    {
        ActionQueue.Dispose();
    }
}
```

## Dispatcher Action Queue

React Native Windows uses the Rx.NET `CoreDispatcherScheduler` (UWP) and `DispatcherScheduler` (WPF) for the scheduler on the dispatcher action queue. This ensures that the actions will be invoked on the app's dispatcher thread. The dispatcher that is used is the one that is found in the current context when the scheduler is instantiated (typically the dispatcher for the main window).

As a React Native Windows user, the best way to interact with the dispatcher action queue is through the `ReactContext.RunOnDispatcherQueueThread` API.

## Layout Action Queue

As noted above, native modules can be initialized with an action queue on which all native methods will be invoked. In the case of the React Native Windows UIManager, a special action queue is created for the native module that runs on a secondary dispatcher thread called the `LayoutActionQueue`. On UWP, this secondary dispatcher is initialized by creating a new, unactivated `CoreApplicationWindow`. On WPF, a new thread is created with an initialized dispatcher.

*Note:* On UWP, the main window's dispatcher thread is used for the `LayoutActionQueue` rather than a secondary dispatcher thread to work around a bug in UWP that causes the app UI to drop when doing native debugging.

## React Native Windows Batching

There are a few cases where you may want to batch activities to prevent backup on the action queues. For example, frequent events like touch and scrolling could overwhelm the JavaScript action queue if they were sent to the JavaScript runtime one at a time. Timers and animations can also create this problem. To work around this, we added the `ReactChoreographer` implementation, which is a simple wrapper around `CompositionTarget.Rendering`. The `ReactChoreographer` has a few events that can be subscribed to:

1. DispatchUICallback
2. NativeAnimatedCallback
3. JavaScriptEventsCallback
4. IdleCallback

These events will each fire (in the order given above) from the main window's dispatcher thread at a rate of 60Hz (roughly every 16ms) when there is "work" to be done. Each subscriber must signal when they have work to be done using the `ReactChoreographer.ActivateCallback(string callbackKey)` method, and must also signal when there is no more work to be done by calling `DeactivateCallback` with the same key passed to `ActivateCallback`. Note, it is important to choose a unique key that won't conflict with other components subscribing to the choreographer (e.g., a GUID). It is also *critical* that you use the `DeactivateCallback` method when no work remains, otherwise the `ReactChoreographer` will continue to tick at 60Hz, wasting CPU cycles and having an impact on battery life. The callback events fire in the order given above.

The way batching is implemented using the `ReactChoreographer` goes as follows. First, the component responsible for batching subscribes to one of the four callbacks above. When an action needs to be taken that should be batched (e.g., a timer is scheduled or an event is dispatched), we call `ActivateCallback` and store the action in some way to be acted on later. When we receive the callback from the `ReactChoreographer`, we process any pending actions, and, no future callbacks are needed, we call `DeactivateCallback`.

The `ReactChoreographer` is also used by the UIManager to queue operations on the view hierarchy.

## Native Module Lifecycle Events

As noted above, native modules can either run on the primary native module action queue or a custom action queue. Here are more details about the threading assumptions for each native module event.

### Native Methods

As stated above, the native methods are called either from the primary native modules action queue, or the custom action queue for the native module.

#### Native Method Callbacks and Promises

Each native method is initially invoked from the assigned action queue, but there are no requirements that the method is synchronous, this is generally left up to the module developer. React Native provides two options to get data back from native methods, `ICallback` and `IPromise`. The native module may mark native methods as `async`, `await` some asynchronous function and call the `ICallback` or `IPromise` function parameter from *any* thread (the callback or promise resolution does not have to occur on the assigned action queue). This means you can and should use `ConfigureAwait(false)` for asynchronous calls in your native methods.

### ReactInstance Lifecycle Events

Each native module is notified when the React instance is initialized (the `Initialize` method) and when the React instance is disposed (the `OnReactInstanceDispose` method). Similar to the native methods, these methods are also called on the action queue assigned to the native module (either the primary native modules action queue or the custom action queue).

### IOnBatchCompleteListener Events

After a JavaScript function or callback is invoked in React, there may be a batch of native operations to perform. By implementing the `IOnBatchCompleteListener`, a native module can receive a callback when all native methods have been queued and executed for that batch on that module. *Note*, because each module can have a custom action queue, there are no guarantees that all native methods have been executed across all modules for that batch, only that the given modules batch is complete. This function is also evaluated on the action queue assigned to the native module (either the primary native modules action queue or the custom action queue).

### ILifecycleEventListener Events

An app may be suspended and resumed at any point in time. It is sometimes useful to know when the app is being suspended and resumed so activities can be stopped while the app is suspended. Native modules can implement the `ILifecycleEventListener` interface to get these lifecycle events. These events always occur on the main window dispatcher thread.
