using System;
using Windows.Foundation;

namespace ReactNative.Tracing
{
    static partial class LoggingActivityBuilderExtensions
    {
        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Boolean value)
        {
            builder.Fields.AddBoolean(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Boolean[] value)
        {
            builder.Fields.AddBooleanArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Double value)
        {
            builder.Fields.AddDouble(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Double[] value)
        {
            builder.Fields.AddDoubleArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Guid value)
        {
            builder.Fields.AddGuid(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Guid[] value)
        {
            builder.Fields.AddGuidArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int16 value)
        {
            builder.Fields.AddInt16(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int16[] value)
        {
            builder.Fields.AddInt16Array(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int32 value)
        {
            builder.Fields.AddInt32(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int32[] value)
        {
            builder.Fields.AddInt32Array(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int64 value)
        {
            builder.Fields.AddInt64(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Int64[] value)
        {
            builder.Fields.AddInt64Array(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Single value)
        {
            builder.Fields.AddSingle(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Single[] value)
        {
            builder.Fields.AddSingleArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, String value)
        {
            builder.Fields.AddString(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, String[] value)
        {
            builder.Fields.AddStringArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, TimeSpan value)
        {
            builder.Fields.AddTimeSpan(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, TimeSpan[] value)
        {
            builder.Fields.AddTimeSpanArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt16 value)
        {
            builder.Fields.AddUInt16(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt16[] value)
        {
            builder.Fields.AddUInt16Array(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt32 value)
        {
            builder.Fields.AddUInt32(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt32[] value)
        {
            builder.Fields.AddUInt32Array(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt64 value)
        {
            builder.Fields.AddUInt64(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, UInt64[] value)
        {
            builder.Fields.AddUInt64Array(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Char value)
        {
            builder.Fields.AddChar16(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Char[] value)
        {
            builder.Fields.AddChar16Array(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, DateTimeOffset value)
        {
            builder.Fields.AddDateTime(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, DateTimeOffset[] value)
        {
            builder.Fields.AddDateTimeArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Rect value)
        {
            builder.Fields.AddRect(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Rect[] value)
        {
            builder.Fields.AddRectArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Point value)
        {
            builder.Fields.AddPoint(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Point[] value)
        {
            builder.Fields.AddPointArray(name, value);
			return builder;
        }

        public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Size value)
        {
            builder.Fields.AddSize(name, value);
			return builder;
        }

		public static LoggingActivityBuilder With(this LoggingActivityBuilder builder, string name, Size[] value)
        {
            builder.Fields.AddSizeArray(name, value);
			return builder;
        }

    }
}
