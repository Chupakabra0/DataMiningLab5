#pragma once
#include <algorithm>
#include <array>
#include <numeric>
#include <map>

class PearsonTable {
private:
	static constexpr auto ALPHA_SIZE = 6u;
	static constexpr auto K_SIZE = 30u;
	static constexpr auto TOTAL_SIZE = PearsonTable::ALPHA_SIZE * PearsonTable::K_SIZE;
public:
	~PearsonTable() = default;

	static PearsonTable& GetInstance() {
		static PearsonTable instance;
		return instance;
	}

	[[nodiscard]] double GetElement(const double sigma, const unsigned k) const {
		auto result = 0.0;

		if (k < this->kTable_.front() || k > this->kTable_.back()) {
			return result;
		}
		if (sigma > this->alphaTable_.front() || sigma < this->alphaTable_.back()) {
			return result;
		}

		const auto lowerBound = this->map_.lower_bound(k)->second.lower_bound(sigma);
		const auto prev = std::prev(lowerBound);

		return std::abs(sigma - prev->first) < std::abs(lowerBound->first - sigma) ? prev->second : lowerBound->second;
	}

private:
	std::array<double, PearsonTable::TOTAL_SIZE> totalTable_{
		0.000, 0.000, 0.003, 3.800, 5.000, 6.600,   // k = 1
		0.020, 0.051, 0.103, 6.000, 7.400, 9.200,   // k = 2
		0.115, 0.216, 0.352, 7.800, 9.400, 11.30,   // k = 3
		0.297, 0.484, 0.711, 9.500, 11.10, 13.30,   // k = 4
		0.554, 0.831, 1.150, 11.10, 12.80, 15.10,   // k = 5
		0.872, 1.240, 1.640, 12.60, 14.40, 16.80,   // k = 6
		1.240, 1.690, 2.170, 14.10, 16.00, 18.50,   // k = 7
		1.650, 2.180, 2.730, 15.50, 17.50, 20.10,   // k = 8
		2.090, 2.700, 3.330, 16.90, 19.00, 21.70,   // k = 9
		2.560, 3.250, 3.940, 18.30, 20.50, 23.20,   // k = 10
		3.050, 3.820, 4.570, 19.70, 21.90, 24.70,   // k = 11
		3.570, 4.400, 5.230, 21.00, 23.30, 26.20,   // k = 12
		4.110, 5.010, 5.890, 22.40, 24.70, 27.70,   // k = 13
		4.660, 5.630, 6.570, 23.70, 26.10, 29.10,   // k = 14
		5.230, 6.260, 7.260, 25.00, 27.50, 30.60,   // k = 15
		5.810, 6.910, 7.960, 26.30, 28.80, 32.00,   // k = 16
		6.410, 7.560, 8.670, 27.60, 30.20, 33.40,   // k = 17
		7.010, 8.230, 9.390, 28.90, 31.50, 34.80,   // k = 18
		7.630, 8.910, 10.10, 30.10, 32.90, 36.20,   // k = 19
		8.260, 9.590, 10.90, 31.40, 34.20, 37.60,   // k = 20
		8.900, 10.30, 11.60, 32.70, 35.50, 38.90,   // k = 21
		9.540, 11.00, 12.30, 33.90, 36.80, 40.30,   // k = 22
		10.20, 11.70, 13.10, 35.20, 38.10, 41.60,   // k = 23
		10.90, 12.40, 13.80, 36.40, 39.40, 43.00,   // k = 24
		11.50, 13.10, 14.60, 37.70, 40.60, 44.30,   // k = 25
		12.20, 13.80, 15.40, 38.90, 41.90, 45.60,   // k = 2
		12.90, 14.60, 16.20, 40.10, 43.20, 47.00,   // k = 27
		13.60, 15.30, 16.90, 41.30, 44.50, 48.30,   // k = 28
		14.30, 16.00, 17.70, 42.60, 45.70, 49.60,   // k = 29
		15.00, 16.80, 18.50, 43.80, 47.00, 50.90   // k = 30
	//  0.990  0.975  0.950  0.050  0.025  0.010
	};

	std::array<unsigned, PearsonTable::K_SIZE> kTable_{};

	std::array<double, PearsonTable::ALPHA_SIZE> alphaTable_{};

	std::map<unsigned, std::map<double, double>> map_{};

	PearsonTable() {
		this->FillKTable();
		this->FillAlphaTable();
		this->FillMap();
	}

	void FillKTable() {
		constexpr auto offset = 30u;
		auto endOffsetIter = std::next(this->kTable_.begin(), offset);
		std::iota(this->kTable_.begin(), endOffsetIter, 1u);
	}

	void FillAlphaTable() {
		this->alphaTable_[0] = 0.990;
		this->alphaTable_[1] = 0.975;
		this->alphaTable_[2] = 0.950;
		this->alphaTable_[3] = 0.050;
		this->alphaTable_[4] = 0.025;
		this->alphaTable_[5] = 0.010;
	}

	void FillMap() {
		auto totalIter = this->totalTable_.begin();

		std::ranges::for_each(std::as_const(this->kTable_), [&totalIter, this](auto k) {
			std::ranges::for_each(std::as_const(this->alphaTable_), [&totalIter, k, this](auto alpha) {
				this->map_[k][alpha] = *totalIter++;
				}
			);
		});
	}
};
