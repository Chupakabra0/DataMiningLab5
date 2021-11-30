#pragma once
#include <numeric>
#include <Eigen/Dense>
#include <vector>

using Eigen::Dynamic;
using Eigen::MatrixXd;


class MathSolver {
public:
	MathSolver() = delete;

	explicit MathSolver(const std::vector<double>& x1, const std::vector<double>& x2, const std::vector<double>& x3)
		: x1(x1), x2(x2), x3(x3), x1Squared(x1.size()), x2Squared(x2.size()),
		x3Squared(x3.size()), xMatrix(MatrixXd::Zero(std::max({ x1.size(), x2.size(), x3.size() }), 3)) {
		this->InitMatrix();
		this->InitSquareVectors();
		this->InitSum();
		this->InitSquareSum();
	}

	[[nodiscard]] double GetX1Sum() const {
		return this->x1Sum;
	}

	[[nodiscard]] double GetX2Sum() const {
		return this->x2Sum;
	}

	[[nodiscard]] double GetX3Sum() const {
		return this->x3Sum;
	}

	[[nodiscard]] double GetX1SquareSum() const {
		return this->x1SquaredSum;
	}

	[[nodiscard]] double GetX2SquareSum() const {
		return this->x2SquaredSum;
	}

	[[nodiscard]] double GetX3SquareSum() const {
		return this->x3SquaredSum;
	}

	[[nodiscard]] std::vector<double> GetX1Data() const {
		return this->x1;
	}

	[[nodiscard]] std::vector<double> GetX2Data() const {
		return this->x2;
	}

	[[nodiscard]] std::vector<double> GetX3Data() const {
		return this->x3;
	}

	[[nodiscard]] std::vector<double> GetX1SquareData() const {
		return this->x1Squared;
	}

	[[nodiscard]] std::vector<double> GetX2SquareData() const {
		return this->x2Squared;
	}

	[[nodiscard]] std::vector<double> GetX3SquareData() const {
		return this->x3Squared;
	}

	[[nodiscard]] MatrixXd GetAllData() const {
		return this->xMatrix;
	}

	[[nodiscard]] double MathExpectX1() const {
		return this->x1Sum / this->x1.size();
	}

	[[nodiscard]] double MathExpectX2() const {
		return this->x2Sum / this->x2.size();
	}

	[[nodiscard]] double MathExpectX3() const {
		return this->x3Sum / this->x3.size();
	}

	[[nodiscard]] double UnbiasedVarianceEstimateX1() const {
		auto result = 0.0;
		std::ranges::for_each(this->x1, [&result, this](auto el) { result += std::pow(el - this->MathExpectX1(), 2.0); });
		return std::pow(result / this->x1.size(), 0.5);
	}

	[[nodiscard]] double UnbiasedVarianceEstimateX2() const {
		auto result = 0.0;
		std::ranges::for_each(this->x2, [&result, this](auto el) { result += std::pow(el - this->MathExpectX2(), 2.0); });
		return std::pow(result / this->x2.size(), 0.5);
	}

	[[nodiscard]] double UnbiasedVarianceEstimateX3() const {
		auto result = 0.0;
		std::ranges::for_each(this->x3, [&result, this](auto el) { result += std::pow(el - this->MathExpectX3(), 2.0); });
		return std::pow(result / this->x2.size(), 0.5);
	}

	[[nodiscard]] MatrixXd NormalizedMatrix() const {
		MatrixXd result = MatrixXd::Zero(this->xMatrix.rows(), this->xMatrix.cols());

		for (auto i = 0u; i < result.rows(); ++i) {
			result(i, 0) = (this->xMatrix(i, 0) - this->MathExpectX1()) / this->UnbiasedVarianceEstimateX1() / std::pow(result.rows(), 0.5);
			result(i, 1) = (this->xMatrix(i, 1) - this->MathExpectX2()) / this->UnbiasedVarianceEstimateX2() / std::pow(result.rows(), 0.5);
			result(i, 2) = (this->xMatrix(i, 2) - this->MathExpectX3()) / this->UnbiasedVarianceEstimateX3() / std::pow(result.rows(), 0.5);
		}

		return result;
	}

	[[nodiscard]] MatrixXd CorrelationMatrix() const {
		const auto normalizedMatrix = this->NormalizedMatrix();
		const auto result = normalizedMatrix.transpose() * normalizedMatrix;
		return result;
	}

	[[nodiscard]] double CorrelationDeterminant() const {
		return this->CorrelationMatrix().determinant();
	}

	[[nodiscard]] double GetXi() const {
		return -(this->xMatrix.rows() - 1.0 - 1.0 / (6 * (2.0 * this->xMatrix.cols() + 5.0))) * log(abs(this->CorrelationDeterminant()));
	}

	[[nodiscard]] MatrixXd CMatrix() const {
		return this->CorrelationMatrix().inverse();
	}

	[[nodiscard]] std::tuple<double, double, double> GetAllFStatistics() const {
		const auto cMatrix = this->CMatrix();

		auto [x1F, x2F, x3F]
			= std::make_tuple((cMatrix(0, 0) - 1.0) * (this->xMatrix.rows() - this->xMatrix.cols()) / (this->xMatrix.cols() - 1),
						      (cMatrix(1, 1) - 1.0) * (this->xMatrix.rows() - this->xMatrix.cols()) / (this->xMatrix.cols() - 1),
						      (cMatrix(2, 2) - 1.0) * (this->xMatrix.rows() - this->xMatrix.cols()) / (this->xMatrix.cols() - 1));

		return std::make_tuple(x1F, x2F, x3F);
	}

	[[nodiscard]] std::tuple<double, double, double> GetAllDetermination() const {
		const auto cMatrix = this->CMatrix();

		auto [x1R, x2R, x3R]
			= std::make_tuple(1.0 - 1.0 / cMatrix(0, 0), 1.0 - 1.0 / cMatrix(1, 1), 1.0 - 1.0 / cMatrix(2, 2));

		return std::make_tuple(x1R, x2R, x3R);
	}

	[[nodiscard]] MatrixXd PartialCorrelationMatrix() const {
		const auto cMatrix = this->CMatrix();
		MatrixXd result = MatrixXd::Zero(cMatrix.rows(), cMatrix.cols());

		for (auto i = 0u; i < result.rows(); ++i) {
			for (auto j = 0u; j < result.cols(); ++j) {
				result(i, j) = -cMatrix(i, j) / std::pow(cMatrix(i, i) * cMatrix(j, j), 0.5);
			}
		}

		return result;
	}

	[[nodiscard]] MatrixXd GetTCriteria() const {
		const auto rMatrix = this->PartialCorrelationMatrix();
		MatrixXd result = MatrixXd::Zero(rMatrix.rows(), rMatrix.cols());

		for (auto i = 0u; i < result.rows(); ++i) {
			for (auto j = 0u; j < result.cols(); ++j) {
				result(i, j) = std::abs(rMatrix(i, j)) * std::pow(this->xMatrix.rows() - this->xMatrix.cols(), 0.5) / std::pow(1 - rMatrix(i, j) * rMatrix(i, j), 0.5);
			}
		}

		return result;
	}

private:
	std::vector<double> x1;
	std::vector<double> x2;
	std::vector<double> x3;

	double x1Sum;
	double x2Sum;
	double x3Sum;

	std::vector<double> x1Squared;
	std::vector<double> x2Squared;
	std::vector<double> x3Squared;

	double x1SquaredSum;
	double x2SquaredSum;
	double x3SquaredSum;

	MatrixXd xMatrix;

	void InitMatrix() {
		std::ranges::for_each(this->x1, [i = 0, this](auto el) mutable { this->xMatrix(i++, 0u) = el; });
		std::ranges::for_each(this->x2, [i = 0, this](auto el) mutable { this->xMatrix(i++, 1u) = el; });
		std::ranges::for_each(this->x3, [i = 0, this](auto el) mutable { this->xMatrix(i++, 2u) = el; });
	}

	void InitSquareVectors() {
		std::ranges::for_each(this->x1, [iter = this->x1Squared.begin()](auto el) mutable { *iter++ = el * el; });
		std::ranges::for_each(this->x2, [iter = this->x2Squared.begin()](auto el) mutable { *iter++ = el * el; });
		std::ranges::for_each(this->x3, [iter = this->x3Squared.begin()](auto el) mutable { *iter++ = el * el; });
	}

	void InitSum() {
		this->x1Sum = std::reduce(this->x1.begin(), this->x1.end(), 0.0, std::plus());
		this->x2Sum = std::reduce(this->x2.begin(), this->x2.end(), 0.0, std::plus());
		this->x3Sum = std::reduce(this->x3.begin(), this->x3.end(), 0.0, std::plus());
	}

	void InitSquareSum() {
		this->x1SquaredSum = std::reduce(this->x1Squared.begin(), this->x1Squared.end(), 0.0, std::plus());
		this->x2SquaredSum = std::reduce(this->x2Squared.begin(), this->x2Squared.end(), 0.0, std::plus());
		this->x3SquaredSum = std::reduce(this->x3Squared.begin(), this->x3Squared.end(), 0.0, std::plus());
	}
};
