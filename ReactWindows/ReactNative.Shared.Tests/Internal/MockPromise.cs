using ReactNative.Bridge;
using System;

namespace ReactNative.Tests
{
    class MockPromise : IPromise
    {
        private const string DefaultError = "EUNSPECIFIED";

        private readonly Action<object> _resolve;
        private readonly Action<string, string, Exception> _reject;
        
        public MockPromise(Action<object> resolve)
            : this(resolve, (_, __, ___)=> { })
        {
        }

        public MockPromise(Action<object> resolve, Action<string, string, Exception> reject)
        {
            _resolve = resolve;
            _reject = reject;
        }

        public void Reject(string code, string message)
        {
            Reject(code, message, default(Exception));
        }

        public void Reject(string message)
        {
            Reject(DefaultError, message, default(Exception));
        }

        public void Reject(string code, Exception e)
        {
            Reject(code, e.Message, e);
        }

        public void Reject(Exception exception)
        {
            Reject(DefaultError, exception.Message, exception);
        }

        public void Reject(string code, string message, Exception e)
        {
            _reject.Invoke(code, message, e);
        }

        public void Resolve(object value)
        {
            _resolve(value);
        }
    }
}
