#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <stdexcept>

template<typename TScalar>
constexpr TScalar DistanceForSpeedChange(TScalar initialSpeed, TScalar finalSpeed, TScalar accelMag) {
	static constexpr TScalar Two = TScalar{ 2 };
	static constexpr TScalar Zero = TScalar{ 0 };
	if (initialSpeed == finalSpeed) return Zero;
	if (accelMag <= Zero) throw std::domain_error("Expected positive acceleration");

	auto [min, max] = std::minmax(finalSpeed, initialSpeed);
	return TScalar{(max * max - min * min) / (Two * accelMag)};
}

template<typename TScalar>
constexpr TScalar TimeForSpeedChange(TScalar initialSpeed, TScalar finalSpeed, TScalar accelMag, TScalar timeScale = 1) {
	static constexpr TScalar Zero = TScalar{ 0 };
	if(initialSpeed == finalSpeed) return Zero;
	if (accelMag <= Zero) throw std::domain_error("Expected positive acceleration");
	auto delta = finalSpeed > initialSpeed 
		? finalSpeed - initialSpeed 
		: initialSpeed - finalSpeed;
	return delta * timeScale / accelMag;
}

template<typename TScalar>
constexpr TScalar PeakSpeedSquaredForDistance(TScalar distance, TScalar initial, TScalar final, TScalar accelMag) {
	static constexpr TScalar Zero = TScalar{ 0 };
	if (accelMag <= Zero) throw std::domain_error("Expected positive acceleration");

	return TScalar{ 
		accelMag * distance + 
		(initial * initial + final * final) / 2 
	};
}

template<typename TScalar>
struct MotionStep {
	TScalar EndSpeed{};
	TScalar Distance{};
};

template<typename TScalar>
MotionStep<TScalar> IntegrateLinearDrag(TScalar initialSpeed, TScalar accel, TScalar responseTime, TScalar elapsed, TScalar timeScale = TScalar{1}) {
	static_assert(!std::integral<TScalar>, "Use floating point for built-in numeric types");
	auto zero = TScalar{};
	if (responseTime <= zero || timeScale <= zero) throw std::domain_error("Expected positive response time and time scale");
	if (elapsed <= zero) return { initialSpeed, zero };

	auto x = [&]() -> double {
		if constexpr(std::floating_point<TScalar>) {
			return static_cast<double>(elapsed) / static_cast<double>(responseTime);
		} else {
			return TScalar::Ratio(elapsed, responseTime);
		}
	}();
	if (!std::isfinite(x) || x < 0.0) throw std::domain_error("Invalid elapsed/response-time ratio");

	auto decay = std::exp(-x);
	double velFactor;
	double distFactor;

	if(x < 1e-4) {
		auto x2 = x * x;
		velFactor = 1.0 - x / 2.0 + x2 / 6.0;
		distFactor = 0.5 - x / 6.0 + x2 / 24.0;
	} else {
		velFactor = -std::expm1(-x) / x;
		distFactor = (1.0 - velFactor) / x;
	}

	auto endSpeed = initialSpeed * decay + accel * elapsed * velFactor / timeScale;
	auto distance = initialSpeed * elapsed * velFactor  / timeScale
		+ accel * elapsed * elapsed * distFactor / timeScale / timeScale;

	return {
		static_cast<TScalar>(endSpeed),
		static_cast<TScalar>(distance)
	};
}