using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;

namespace ReactNative.Animated
{
    class DecayAnimationDriver : AnimationDriver
    {
        private readonly double _velocity;
        private readonly double _deceleration;

        private long _startFrameTimeMillis = -1;
        private double _fromValue;
        private double _lastValue;

        public DecayAnimationDriver(int id, ValueAnimatedNode animatedValue, ICallback endCallback, JObject config)
            : base(id, animatedValue, endCallback)
        {
            _velocity = config.Value<double>("velocity");
            _deceleration = config.Value<double>("deceleration");
        }

        public override void RunAnimationStep(TimeSpan renderingTime)
        {
            long frameTimeMillis = renderingTime.Ticks / 10000;
            if (_startFrameTimeMillis == -1)
            {
                // since this is the first animation step, consider the start to be on the previous frame
                _startFrameTimeMillis = frameTimeMillis - 16;
                _fromValue = AnimatedValue.Value;
                _lastValue = AnimatedValue.Value;
            }

            var value = _fromValue +
                (_velocity / (1 - _deceleration)) *
                    (1 - Math.Exp(-(1 - _deceleration) * (frameTimeMillis - _startFrameTimeMillis)));

            if (Math.Abs(_lastValue - value) < 0.1)
            {
                HasFinished = true;
                return;
            }

            _lastValue = value;
            AnimatedValue.Value = value;
        }
    }
}
