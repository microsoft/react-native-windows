using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;

namespace ReactNative.Animated
{
    internal class SpringAnimationDriver : AnimationDriver
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
        private double _springFriction;
        private double _springTension;
        private bool _overshootClampingEnabled;

        // all physics simulation objects are reused in each processing pass
        private PhysicsState _currentState = new PhysicsState();
        private PhysicsState _previousState = new PhysicsState();
        private PhysicsState _tempState = new PhysicsState();
        private double _startValue;
        private double _endValue;
        // thresholds for determining when the spring is at rest
        private double _restSpeedThreshold;
        private double _displacementFromRestThreshold;
        private double _timeAccumulator = 0;
        
        public SpringAnimationDriver(int id, ValueAnimatedNode animatedValue, ICallback endCallback, JObject config)
            : base(id, animatedValue, endCallback)
        {
            _springFriction = config.Value<double>("friction");
            _springTension = config.Value<double>("tension");
            _currentState.Velocity = config.Value<double>("initialVelocity");
            _endValue = config.Value<double>("toValue");
            _restSpeedThreshold = config.Value<double>("restSpeedThreshold");
            _displacementFromRestThreshold = config.Value<double>("restDisplacementThreshold");
            _overshootClampingEnabled = config.Value<bool>("overshootClamping");
        }

        public override void RunAnimationStep(TimeSpan renderingTime)
        {
            var frameTimeMillis = renderingTime.Ticks / 10000;
            if (!_springStarted)
            {
                _startValue = _currentState.Position = AnimatedValue.Value;
                _lastTime = frameTimeMillis;
                _springStarted = true;
            }

            Advance((frameTimeMillis - _lastTime) / 1000.0);
            _lastTime = frameTimeMillis;
            AnimatedValue.Value = _currentState.Position;

            HasFinished = IsAtRest();
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
                    _springTension == 0);
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
            return _springTension > 0 &&
                ((_startValue < _endValue && _currentState.Position > _endValue) ||
                    (_startValue > _endValue && _currentState.Position < _endValue));
        }

        /// <summary>
        /// Linear interpolation between the previous and current physics state
        /// based on the amount of timestep remaining after processing the
        /// rendering delta time in timestep sized chunks.
        /// </summary>
        /// <param name="alpha">
        /// Value from 0 to 1, where 0 is the previous state, 1 is the current state.
        /// </param>
        private void Interpolate(double alpha)
        {
            _currentState.Position = _currentState.Position * alpha + _currentState.Position * (1 - alpha);
            _currentState.Velocity = _currentState.Velocity * alpha + _currentState.Velocity * (1 - alpha);
        }

        /// <summary>
        /// Advance the physics simulation in <see cref="SolverTimestepSec"/>
        /// sized chunks to fulfill the required <paramref name="realDeltaTime"/>.
        /// </summary>
        /// <param name="realDeltaTime">Clock drift.</param>
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

            var tension = _springTension;
            var friction =_springFriction;

            var position = _currentState.Position;
            var velocity = _currentState.Velocity;
            var tempPosition = _tempState.Position;
            var tempVelocity = _tempState.Velocity;

            double aVelocity, aAcceleration;
            double bVelocity, bAcceleration;
            double cVelocity, cAcceleration;
            double dVelocity, dAcceleration;

            double dxdt, dvdt;

            // iterate over the true time
            while (_timeAccumulator >= SolverTimestepSec)
            {
                _timeAccumulator -= SolverTimestepSec;

                if (_timeAccumulator < SolverTimestepSec)
                {
                    // This will be the last iteration. Remember the previous state in case we need to
                    // interpolate
                    _previousState.Position = position;
                    _previousState.Velocity = velocity;
                }

                // Perform an RK4 integration to provide better detection of the acceleration curve via
                // sampling of Euler integrations at 4 intervals feeding each derivative into the calculation
                // of the next and taking a weighted sum of the 4 derivatives as the final output.

                // This math was inlined since it made for big performance improvements when advancing several
                // springs in one pass of the BaseSpringSystem.

                // The initial derivative is based on the current velocity and the calculated acceleration
                aVelocity = velocity;
                aAcceleration = (tension * (_endValue - tempPosition)) - friction * velocity;

                // Calculate the next derivatives starting with the last derivative and integrating over the
                // timestep
                tempPosition = position + aVelocity * SolverTimestepSec * 0.5;
                tempVelocity = velocity + aAcceleration * SolverTimestepSec * 0.5;
                bVelocity = tempVelocity;
                bAcceleration = (tension * (_endValue - tempPosition)) - friction * tempVelocity;

                tempPosition = position + bVelocity * SolverTimestepSec * 0.5;
                tempVelocity = velocity + bAcceleration * SolverTimestepSec * 0.5;
                cVelocity = tempVelocity;
                cAcceleration = (tension * (_endValue - tempPosition)) - friction * tempVelocity;

                tempPosition = position + cVelocity * SolverTimestepSec;
                tempVelocity = velocity + cAcceleration * SolverTimestepSec;
                dVelocity = tempVelocity;
                dAcceleration = (tension * (_endValue - tempPosition)) - friction * tempVelocity;

                // Take the weighted sum of the 4 derivatives as the final output.
                dxdt = 1.0 / 6.0 * (aVelocity + 2.0 * (bVelocity + cVelocity) + dVelocity);
                dvdt = 1.0 / 6.0 * (aAcceleration + 2.0 * (bAcceleration + cAcceleration) + dAcceleration);

                position += dxdt * SolverTimestepSec;
                velocity += dvdt * SolverTimestepSec;
            }

            _tempState.Position = tempPosition;
            _tempState.Velocity = tempVelocity;

            _currentState.Position = position;
            _currentState.Velocity = velocity;

            if (_timeAccumulator > 0)
            {
                Interpolate(_timeAccumulator / SolverTimestepSec);
            }

            // End the spring immediately if it is overshooting and overshoot clamping is enabled.
            // Also make sure that if the spring was considered within a resting threshold that it's now
            // snapped to its end value.
            if (IsAtRest() || (_overshootClampingEnabled && IsOvershooting()))
            {
                // Don't call setCurrentValue because that forces a call to onSpringUpdate
                if (tension > 0)
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
