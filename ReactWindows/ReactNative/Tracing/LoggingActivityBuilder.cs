// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Foundation.Diagnostics;

namespace ReactNative.Tracing
{
    partial class LoggingActivityBuilder : ITraceBuilder
    {
        private readonly ILoggingTarget _target;
        private readonly string _name;
        private readonly LoggingLevel _level;
        private readonly LoggingOptions _options;

        private LoggingFields _fields;

        public LoggingActivityBuilder(ILoggingTarget target, string name, LoggingLevel level, LoggingOptions options)
        {
            _target = target;
            _name = name;
            _level = level;
            _options = options;
        }

        private LoggingFields Fields
        {
            get
            {
                if (_fields == null)
                {
                    _fields = new LoggingFields();
                }

                return _fields;
            }
        }

        public IDisposable Start()
        {
            return _target.StartActivity(_name, _fields, _level, _options);
        }
    }
}
