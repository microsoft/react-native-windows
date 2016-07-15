using System;
using Windows.Foundation.Diagnostics;

namespace ReactNative.Tracing
{
    class LoggingActivityBuilder
    {
        private readonly ILoggingTarget _target;
        private readonly string _name;
        private readonly LoggingLevel? _level;
        private readonly LoggingOptions _options;

        private LoggingFields _fields;

        public LoggingActivityBuilder(ILoggingTarget target, string name, LoggingLevel level, LoggingOptions options)
        {
            _target = target;
            _name = name;
            _level = level;
            _options = options;
        }

        public LoggingFields Fields
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

        public LoggingActivity Create()
        {
            if (_fields == null && !_level.HasValue)
            {
                return _target.StartActivity(_name);
            }
            else if (!_level.HasValue)
            {
                return _target.StartActivity(_name, _fields);
            }
            else if (_options == null)
            {
                return _target.StartActivity(_name, _fields, _level.Value);
            }
            else
            {
                return _target.StartActivity(_name, _fields, _level.Value, _options);
            }
        }
    }
}
