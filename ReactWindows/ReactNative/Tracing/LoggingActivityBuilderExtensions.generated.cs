using System;
using Windows.Foundation;

namespace ReactNative.Tracing
{
    static partial class LoggingFieldsExtensions
    {
        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Boolean value)
        {
            if (builder != null)
            {
                builder.Fields.AddBoolean(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Boolean[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddBooleanArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Double value)
        {
            if (builder != null)
            {
                builder.Fields.AddDouble(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Double[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddDoubleArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Guid value)
        {
            if (builder != null)
            {
                builder.Fields.AddGuid(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Guid[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddGuidArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int16 value)
        {
            if (builder != null)
            {
                builder.Fields.AddInt16(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int16[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddInt16Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int32 value)
        {
            if (builder != null)
            {
                builder.Fields.AddInt32(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int32[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddInt32Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int64 value)
        {
            if (builder != null)
            {
                builder.Fields.AddInt64(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int64[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddInt64Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Single value)
        {
            if (builder != null)
            {
                builder.Fields.AddSingle(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Single[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddSingleArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, String value)
        {
            if (builder != null)
            {
                builder.Fields.AddString(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, String[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddStringArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, TimeSpan value)
        {
            if (builder != null)
            {
                builder.Fields.AddTimeSpan(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, TimeSpan[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddTimeSpanArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt16 value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt16(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt16[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt16Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt32 value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt32(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt32[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt32Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt64 value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt64(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt64[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt64Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Byte value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt8(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Byte[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddUInt8Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Char value)
        {
            if (builder != null)
            {
                builder.Fields.AddChar16(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Char[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddChar16Array(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, DateTimeOffset value)
        {
            if (builder != null)
            {
                builder.Fields.AddDateTime(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, DateTimeOffset[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddDateTimeArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Rect value)
        {
            if (builder != null)
            {
                builder.Fields.AddRect(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Rect[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddRectArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Point value)
        {
            if (builder != null)
            {
                builder.Fields.AddPoint(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Point[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddPointArray(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Size value)
        {
            if (builder != null)
            {
                builder.Fields.AddSize(name, value);
            }

            return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Size[] value)
        {
            if (builder != null)
            {
                builder.Fields.AddSizeArray(name, value);
            }

            return builder;
        }

        public static IDisposable Start(this LoggingActivityBuilder builder)
        {
            return builder?.Create();
        }
    }
}
