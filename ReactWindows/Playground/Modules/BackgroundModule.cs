using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using Windows.ApplicationModel.Background;

namespace Playground.Modules
{
    class BackgroundModule : NativeModuleBase
    {
        public override string Name => "Background";

        private readonly IDictionary<string, BackgroundTaskDeferral> _deferrals = new Dictionary<string, BackgroundTaskDeferral>();

        [ReactMethod]
        public async void registerSystemTrigger(string name, IPromise promise)
        {
            var requestTask = await BackgroundExecutionManager.RequestAccessAsync().AsTask().ConfigureAwait(false);
            var builder = new BackgroundTaskBuilder();
            builder.Name = name;

            var trigger = new SystemTrigger(SystemTriggerType.TimeZoneChange, false);
            builder.SetTrigger(trigger);

            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                task.Value.Unregister(true);
            }

            var registration = builder.Register();
            var taskId = registration.TaskId.ToString();
            promise.Resolve(taskId);
            Debug.WriteLine($"[ReactNative] Registered {taskId}.");
        }

        [ReactMethod]
        public void unregister(string taskId)
        {
            var guid = Guid.Parse(taskId);
            var registration = default(IBackgroundTaskRegistration);
            if (BackgroundTaskRegistration.AllTasks.TryGetValue(guid, out registration))
            {
                registration.Unregister(true);
                Debug.WriteLine($"[ReactNative] Unregistered {taskId}.");
            }
        }

        [ReactMethod]
        public void completeDeferral(string deferralId)
        {
            var deferral = default(BackgroundTaskDeferral);
            if (!_deferrals.TryGetValue(deferralId, out deferral))
            {
                throw new InvalidOperationException("Deferral key has not been registered.");
            }

            deferral.Complete();
            Debug.WriteLine($"[ReactNative] Background task deferral completed.");
        }

        public void RegisterDeferral(string id, BackgroundTaskDeferral deferral)
        {
            _deferrals.Add(id, deferral);
            Debug.WriteLine($"[ReactNative] Deferral registered {id}.");
        }
    }

}
