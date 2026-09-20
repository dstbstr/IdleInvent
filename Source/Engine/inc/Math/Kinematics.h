#pragma once

#include <algorithm>
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
