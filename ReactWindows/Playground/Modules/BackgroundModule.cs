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

        private readonly IDictionary<string, ApplicationTrigger> _applicationTriggers = new Dictionary<string, ApplicationTrigger>();

        [ReactMethod]
        public async void registerApplicationTrigger(string name, string triggerId, IPromise promise)
        {
            var requestTask = await BackgroundExecutionManager.RequestAccessAsync().AsTask().ConfigureAwait(false);
            var builder = new BackgroundTaskBuilder();
            builder.Name = name;
            if (_applicationTriggers.ContainsKey(name))
            {
                promise.Reject("Trigger already exists.");
                return;
            }

            var trigger = new ApplicationTrigger();
            _applicationTriggers.Add(triggerId, trigger);
            builder.SetTrigger(trigger);

            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                task.Value.Unregister(true);
            }

            var registration = builder.Register();
            promise.Resolve(registration.TaskId.ToString());
        }

        [ReactMethod]
        public async void registerSystemTrigger(string name, IPromise promise)
        {
            var requestTask = await BackgroundExecutionManager.RequestAccessAsync().AsTask().ConfigureAwait(false);
            var builder = new BackgroundTaskBuilder();
            builder.Name = name;
            if (_applicationTriggers.ContainsKey(name))
            {
                promise.Reject("Trigger already exists.");
                return;
            }

            var trigger = new SystemTrigger(SystemTriggerType.TimeZoneChange, false);
            builder.SetTrigger(trigger);

            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                task.Value.Unregister(true);
            }

            var registration = builder.Register();
            promise.Resolve(registration.TaskId.ToString());

        }

        [ReactMethod]
        public async void trigger(string triggerId, IPromise promise)
        {
            var trigger = default(ApplicationTrigger);
            if (!_applicationTriggers.TryGetValue(triggerId, out trigger))
            {
                promise.Reject("Trigger does not exist.");
                return;
            }

            var result = await trigger.RequestAsync();
            promise.Resolve(result);
        }

        [ReactMethod]
        public void unregister(string taskId)
        {
            var guid = Guid.Parse(taskId);
            var registration = default(IBackgroundTaskRegistration);
            if (BackgroundTaskRegistration.AllTasks.TryGetValue(guid, out registration))
            {
                registration.Unregister(true);
                Debug.WriteLine($"[ReactNative] Unregistered {taskId}");
            }
        }
    }

}
