#pragma once
#include "IDataGetter.hpp"

class TestDataGetter : public IDataGetter {
public:
	~TestDataGetter() override = default;

	[[nodiscard]] std::vector<double> GetDataX1() const override {
		return { 10.37, 10.37, 10.28, 10.25, 11.72, 11.28, 11.45, 10.40, 11.60, 9.80, 9.81, 8.90, 9.84, 12.70, 12.27, 12.08, 14.90, 15.02 };
	}

	[[nodiscard]] std::vector<double> GetDataX2() const override {
		return { 9.87, 11.08, 11.08, 9.08, 10.05, 20.18, 10.69, 13.90, 14.50, 14.70, 10.80, 15.06, 13.27, 16.20, 15.07, 15.20, 17.90, 20.37 };
	}

	[[nodiscard]] std::vector<double> GetDataX3() const override {
		return { 8.20, 9.80, 10.10, 5.80, 9.50, 15.70, 11.50, 10.60, 11.40, 10.10, 9.40, 8.10, 10.80, 11.50, 10.20, 11.50, 12.90, 21.40 };
	}

	[[nodiscard]] double GetFreedom() const override {
		return 0.95;
	}
};