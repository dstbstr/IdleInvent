#pragma once

#include <Platform/NumTypes.h>

#include <random>

struct IRandom {
	virtual ~IRandom() = default;

	virtual size_t GetNext() = 0;
	virtual size_t GetNext(size_t max) = 0;
	virtual size_t GetNext(size_t min, size_t max) = 0;

	virtual f32 GetNextFloat() = 0;
};

struct DefaultRandom : IRandom {
	size_t GetNext() override {
		return dist(gen);
	}
	size_t GetNext(size_t max) override {
		return dist(gen) % max;
	}
	size_t GetNext(size_t min, size_t max) override {
		return dist(gen) % (max - min) + min;
	}

	f32 GetNextFloat() override {
		return static_cast<f32>(dist(gen)) / static_cast<f32>(std::numeric_limits<size_t>::max());
	}

private:
	std::random_device rd{};
	std::mt19937 gen{ rd()};
	std::uniform_int_distribution<size_t> dist{ 0, std::numeric_limits<size_t>::max() };
};