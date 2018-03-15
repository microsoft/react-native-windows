//
// This file is generated with LoggingActivityBuilderExtensions.tt
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Tracing
{
    partial class LoggingActivityBuilder : ITraceBuilder
    {
        public ITraceBuilder With(string name, Boolean value)
        {
            Fields.AddBoolean(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Boolean[] value)
        {
            Fields.AddBooleanArray(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Double value)
        {
            Fields.AddDouble(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Double[] value)
        {
            Fields.AddDoubleArray(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Guid value)
        {
            Fields.AddGuid(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Guid[] value)
        {
            Fields.AddGuidArray(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Int16 value)
        {
            Fields.AddInt16(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Int16[] value)
        {
            Fields.AddInt16Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Int32 value)
        {
            Fields.AddInt32(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Int32[] value)
        {
            Fields.AddInt32Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Int64 value)
        {
            Fields.AddInt64(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Int64[] value)
        {
            Fields.AddInt64Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Single value)
        {
            Fields.AddSingle(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Single[] value)
        {
            Fields.AddSingleArray(name, value);
            return this;
        }

        public ITraceBuilder With(string name, String value)
        {
            Fields.AddString(name, value);
            return this;
        }

        public ITraceBuilder With(string name, String[] value)
        {
            Fields.AddStringArray(name, value);
            return this;
        }

        public ITraceBuilder With(string name, TimeSpan value)
        {
            Fields.AddTimeSpan(name, value);
            return this;
        }

        public ITraceBuilder With(string name, TimeSpan[] value)
        {
            Fields.AddTimeSpanArray(name, value);
            return this;
        }

        public ITraceBuilder With(string name, UInt16 value)
        {
            Fields.AddUInt16(name, value);
            return this;
        }

        public ITraceBuilder With(string name, UInt16[] value)
        {
            Fields.AddUInt16Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, UInt32 value)
        {
            Fields.AddUInt32(name, value);
            return this;
        }

        public ITraceBuilder With(string name, UInt32[] value)
        {
            Fields.AddUInt32Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, UInt64 value)
        {
            Fields.AddUInt64(name, value);
            return this;
        }

        public ITraceBuilder With(string name, UInt64[] value)
        {
            Fields.AddUInt64Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Byte value)
        {
            Fields.AddUInt8(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Byte[] value)
        {
            Fields.AddUInt8Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Char value)
        {
            Fields.AddChar16(name, value);
            return this;
        }

        public ITraceBuilder With(string name, Char[] value)
        {
            Fields.AddChar16Array(name, value);
            return this;
        }

        public ITraceBuilder With(string name, DateTimeOffset value)
        {
            Fields.AddDateTime(name, value);
            return this;
        }

        public ITraceBuilder With(string name, DateTimeOffset[] value)
        {
            Fields.AddDateTimeArray(name, value);
            return this;
        }

    }
}
