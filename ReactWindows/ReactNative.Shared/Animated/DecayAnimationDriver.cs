// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Json;
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
        private int _iterations;
        private int _currentLoop;

        public DecayAnimationDriver(int id, ValueAnimatedNode animatedValue, ICallback endCallback, JObject config)
            : base(id, animatedValue, endCallback)
        {
            _velocity = config.Value<double>("velocity");
            _deceleration = config.Value<double>("deceleration");
            _iterations = config.ContainsKey("iterations") ? config.Value<int>("iterations") : 1;
            _currentLoop = 1;
            HasFinished = _iterations == 0;
        }

        public override void RunAnimationStep(TimeSpan renderingTime)
        {
            long frameTimeMillis = renderingTime.Ticks / 10000;
            if (_startFrameTimeMillis == -1)
            {
                // since this is the first animation step, consider the start to be on the previous frame
                _startFrameTimeMillis = frameTimeMillis - 16;
                if (_fromValue == _lastValue)
                {
                    // First iteration, assign _fromValue based on AnimatedValue
                    _fromValue = AnimatedValue.RawValue;
                }
                else
                {
                    // Not the first iteration, reset AnimatedValue based on _fromValue
                    AnimatedValue.RawValue = _fromValue;
                }

                _lastValue = AnimatedValue.RawValue;
            }

            var value = _fromValue +
                (_velocity / (1 - _deceleration)) *
                    (1 - Math.Exp(-(1 - _deceleration) * (frameTimeMillis - _startFrameTimeMillis)));

            if (Math.Abs(_lastValue - value) < 0.1)
            {
                if (_iterations == -1 || _currentLoop < _iterations)
                {
                    // looping animation, return to start
                    // set _startFrameTimeMillis to -1 to reset instance variables on the next RunAnimationStep
                    _startFrameTimeMillis = -1;
                    _currentLoop++;
                }
                else
                {
                    // animation has completed
                    HasFinished = true;
                    return;
                }
            }

            _lastValue = value;
            AnimatedValue.RawValue = value;
        }
    }
}
