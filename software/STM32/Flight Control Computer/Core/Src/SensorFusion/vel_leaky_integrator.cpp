/*
 * vel_leaky_integrator.cpp
 *
 *  Created on: Aug 4, 2026
 *      Author: Nikolai Philipenko
 */

#include <cmath>

#include "SensorFusion/vel_leaky_integrator.hpp"


VelocityLeakyIntegrator::VelocityLeakyIntegrator(PMW3901& optical_flow, BMI088& imu)
	: optical_flow(optical_flow),
	  imu(imu) {}

std::array<float, 2> VelocityLeakyIntegrator::update(const MFX_output_t& state, float altitude, float deltatime_s)
{
	if (deltatime_s <= 0.0f) return velocity;

	// Minimum altitude clamp: 10 cm
	float valid_altitude = std::max(0.1f, altitude);

	if (!initialized)
	{
		prev_state = state;
		initialized = true;
	}

	// Get optical flow data
	std::array<float, 2> flow_rate = optical_flow.get_flow_rate();

	// Compensate flow rates for body motion
	std::array<float, 3> angular_velocity = imu.get_angular_velocities();

	// TODO: VERIFY MAPPING
	std::array<float, 2> flow_rate_compensated{};
	flow_rate_compensated[0] = flow_rate[0] - angular_velocity[0];
	flow_rate_compensated[1] = flow_rate[1] - angular_velocity[1];

	// Compute camera velocity
	std::array<float, 2> camera_velocity{};
	camera_velocity[0] = flow_rate_compensated[0] * valid_altitude;
	camera_velocity[1] = flow_rate_compensated[1] * valid_altitude;

	// Integrate accelerations (gravity removed) to get delta imu velocity (trapezoidal rule)
	std::array<float, 2> imu_delta_velocity{};
	imu_delta_velocity[0] = 0.5f * (state.linear_acceleration[0] + prev_state.linear_acceleration[0]) * deltatime_s;
	imu_delta_velocity[1] = 0.5f * (state.linear_acceleration[1] + prev_state.linear_acceleration[1]) * deltatime_s;

	prev_state = state;

	// Apply Leaky Integrator to fuse velocities

	const float tau = VEL_SETTLING_TIME / 4.0f;
	const float velocity_leak_alpha = std::exp(-deltatime_s / tau);

	velocity[0] = imu_delta_velocity[0] + velocity[0] * velocity_leak_alpha + (1.0f - velocity_leak_alpha) * camera_velocity[0];
	velocity[1] = imu_delta_velocity[1] + velocity[1] * velocity_leak_alpha + (1.0f - velocity_leak_alpha) * camera_velocity[1];

	return velocity;
}

