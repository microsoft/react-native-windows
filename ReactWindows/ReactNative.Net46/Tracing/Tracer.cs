using System;

namespace ReactNative.Tracing
{
    class NullDisposable : IDisposable
    {
        public void Dispose()
        {

        }
    }
    public class NullLoggingActivityBuilder
    {
        public IDisposable Start()
        {
            return new NullDisposable();
        }
    }
    public static class Tracer
    {
        public const int TRACE_TAG_REACT_BRIDGE = 0;
        public const int TRACE_TAG_REACT_APPS = 1;
        public const int TRACE_TAG_REACT_VIEW = 2;

        public static NullLoggingActivityBuilder Trace(int tag, string name)
        {
            return new NullLoggingActivityBuilder();
        }

        public static void Write(int tag, string eventName)
        {
        }

        public static void Error(int tag, string eventName, Exception ex)
        {
        }
    }
}