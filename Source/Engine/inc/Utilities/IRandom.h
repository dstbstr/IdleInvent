#pragma once

#include <Platform/NumTypes.h>

#include <random>

struct IRandom {
	virtual ~IRandom() = default;

	virtual u64 GetNextU() = 0;
    virtual u64 GetNextU(u64 max) = 0;
    virtual u64 GetNextU(u64 min, u64 max) = 0;

    virtual s64 GetNext() = 0;
    virtual s64 GetNext(s64 max) = 0;
    virtual s64 GetNext(s64 min, s64 max) = 0;

	virtual f32 GetNextFloat() = 0;
};

struct DefaultRandom : IRandom {
	s64 GetNext() override { return dist(gen); }
	s64 GetNext(s64 max) override { return dist(gen) % max;	}
	s64 GetNext(s64 min, s64 max) override { return dist(gen) % (max - min) + min; }

	u64 GetNextU() override { return static_cast<u64>(dist(gen)); }
    u64 GetNextU(u64 max) override { return static_cast<u64>(dist(gen)) % max; }
    u64 GetNextU(u64 min, u64 max) override { return static_cast<u64>(dist(gen)) % (max - min) + min; }

	f32 GetNextFloat() override {
		return static_cast<f32>(dist(gen)) / static_cast<f32>(std::numeric_limits<size_t>::max());
	}

private:
	std::random_device rd{};
	std::mt19937 gen{ rd()};
	std::uniform_int_distribution<s64> dist{ 0, std::numeric_limits<s64>::max() };
};