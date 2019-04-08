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
    class SpringAnimationDriver : AnimationDriver
    {
        /// <summary>
        /// Maximum amount of time to simulate per physics iteration in seconds (4 frames at 60 FPS).
        /// </summary>
        private const double MaxDeltaTimeSec = 0.064;

        /// <summary>
        /// Fixed timestep to use in the physics solver in seconds.
        /// </summary>
        private const double SolverTimestepSec = 0.001;

        private long _lastTime;
        private bool _springStarted;

        // configuration
        private double _springStiffness;
        private double _springDamping;
        private double _springMass;
        private double _initialVelocity;
        private bool _overshootClampingEnabled;

        // all physics simulation objects are reused in each processing pass
        private PhysicsState _currentState = new PhysicsState();
        private double _startValue;
        private double _endValue;
        // thresholds for determining when the spring is at rest
        private double _restSpeedThreshold;
        private double _displacementFromRestThreshold;
        private double _timeAccumulator = 0;
        // for controlling loop
        private int _iterations;
        private int _currentLoop = 0;
        private double _originalValue;
        
        public SpringAnimationDriver(int id, ValueAnimatedNode animatedValue, ICallback endCallback, JObject config)
            : base(id, animatedValue, endCallback)
        {
            _springStiffness = config.Value<double>("stiffness");
            _springDamping = config.Value<double>("damping");
            _springMass = config.Value<double>("mass");
            _initialVelocity = config.Value<double>("initialVelocity");
            _currentState.Velocity = _initialVelocity;
            _endValue = config.Value<double>("toValue");
            _restSpeedThreshold = config.Value<double>("restSpeedThreshold");
            _displacementFromRestThreshold = config.Value<double>("restDisplacementThreshold");
            _overshootClampingEnabled = config.Value<bool>("overshootClamping");
            _iterations = config.ContainsKey("iterations") ? config.Value<int>("iterations") : 1;
            HasFinished = _iterations == 0;
        }

        public override void RunAnimationStep(TimeSpan renderingTime)
        {
            var frameTimeMillis = renderingTime.Ticks / 10000;
            if (!_springStarted)
            {
                if (_currentLoop == 0)
                {
                    _originalValue = AnimatedValue.RawValue;
                    _currentLoop = 1;
                }
                _startValue = _currentState.Position = AnimatedValue.RawValue;
                _lastTime = frameTimeMillis;
                _timeAccumulator = 0.0;
                _springStarted = true;
            }

            Advance((frameTimeMillis - _lastTime) / 1000.0);
            _lastTime = frameTimeMillis;
            AnimatedValue.RawValue = _currentState.Position;
            if (IsAtRest())
            {
                if (_iterations == -1 || _currentLoop < _iterations)
                {
                    // looping animation, return to start
                    _springStarted = false;
                    AnimatedValue.RawValue = _originalValue;
                    _currentLoop++;
                }
                else
                {
                    // animation has completed
                    HasFinished = IsAtRest();
                }
            }
        }

        /// <summary>
        /// Gets the displacement from rest for a given physics state.
        /// </summary>
        /// <param name="state">The state to measure from.</param>
        /// <returns>The distance displaced by.</returns>
        private double GetDisplacementDistanceForState(PhysicsState state)
        {
            return Math.Abs(_endValue - state.Position);
        }

        /// <summary>
        /// Check if current state is at rest.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the spring is at rest, otherwise <code>false</code>.
        /// </returns>
        private bool IsAtRest()
        {
            return Math.Abs(_currentState.Velocity) <= _restSpeedThreshold &&
                (GetDisplacementDistanceForState(_currentState) <= _displacementFromRestThreshold ||
                    _springStiffness == 0);
        }

        /// <summary>
        /// Check if the spring is overshooting beyond its target.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the spring is overshooting its target,
        /// otherwise <code>false</code>.
        /// </returns>
        private bool IsOvershooting()
        {
            return _springStiffness > 0 &&
                ((_startValue < _endValue && _currentState.Position > _endValue) ||
                    (_startValue > _endValue && _currentState.Position < _endValue));
        }

        private void Advance(double realDeltaTime)
        {
            if (IsAtRest())
            {
                return;
            }

            // clamp the amount of realDeltaTime to avoid stuttering in the UI.
            // We should be able to catch up in a subsequent advance if necessary.
            var adjustedDeltaTime = realDeltaTime;
            if (realDeltaTime > MaxDeltaTimeSec)
            {
                adjustedDeltaTime = MaxDeltaTimeSec;
            }

            _timeAccumulator += adjustedDeltaTime;

            var c = _springDamping;
            var m = _springMass;
            var k = _springStiffness;
            var v0 = -_initialVelocity;

            var zeta = c / (2 * Math.Sqrt(k * m));
            var omega0 = Math.Sqrt(k / m);
            var omega1 = omega0 * Math.Sqrt(1.0 - (zeta * zeta));
            var x0 = _endValue - _startValue;

            double velocity;
            double position;
            var t = _timeAccumulator;
            if (zeta < 1)
            {
                // Under damped
                double envelope = Math.Exp(-zeta * omega0 * t);
                position =
                  _endValue -
                    envelope *
                      ((v0 + zeta * omega0 * x0) / omega1 * Math.Sin(omega1 * t) +
                        x0 * Math.Cos(omega1 * t));
                // This looks crazy -- it's actually just the derivative of the
                // oscillation function
                velocity =
                  zeta *
                    omega0 *
                    envelope *
                    (Math.Sin(omega1 * t) * (v0 + zeta * omega0 * x0) / omega1 +
                      x0 * Math.Cos(omega1 * t)) -
                    envelope *
                      (Math.Cos(omega1 * t) * (v0 + zeta * omega0 * x0) -
                        omega1 * x0 * Math.Sin(omega1 * t));
            }
            else
            {
                // Critically damped spring
                double envelope = Math.Exp(-omega0 * t);
                position = _endValue - envelope * (x0 + (v0 + omega0 * x0) * t);
                velocity =
                  envelope * (v0 * (t * omega0 - 1) + t * x0 * (omega0 * omega0));
            }

            _currentState.Position = position;
            _currentState.Velocity = velocity;

            // End the spring immediately if it is overshooting and overshoot clamping is enabled.
            // Also make sure that if the spring was considered within a resting threshold that it's now
            // snapped to its end value.
            if (IsAtRest() || (_overshootClampingEnabled && IsOvershooting()))
            {
                // Don't call setCurrentValue because that forces a call to onSpringUpdate
                if (_springStiffness > 0)
                {
                    _startValue = _endValue;
                    _currentState.Position = _endValue;
                }
                else
                {
                    _endValue = _currentState.Position;
                    _startValue = _endValue;
                }
                _currentState.Velocity = 0;
            }
        }

        struct PhysicsState
        {
            public double Position;
            public double Velocity;
        }
    }
}
