#pragma once
#include "MainHeader.hpp"

class Main {
private:
	static const std::pair <const char*, const char*> FILES_ARG_STR;

public:
	Main() = delete;

	Main(const Main&) = delete;

	Main(Main&&) = default;

	explicit Main(int argc, char* argv[]) {
		this->InitArgv(argc, argv);
	}

	~Main() = default;

	int LaunchMain() {
		Main::SetLocale(1251u);

		try {
			this->ParseArgs();
		}
		catch (const std::exception&) {
			fmt::print(fmt::fg(fmt::color::orange),
				"ВНИМАНИЕ! Добавьте файлы с входными данными...\nПример: {} {} fileName.json\n",
				this->argv_.front(), Main::FILES_ARG_STR.second);

			return EXIT_FAILURE;
		}

		for (auto filePath : this->filePaths_) {

			std::fstream file(filePath, std::ios_base::in);
			const std::unique_ptr<IDataGetter> dataGetter = std::make_unique<JsonDataGetter>(file);

			const auto [x1, x2, x3] =
				std::make_tuple(dataGetter->GetDataX1(), dataGetter->GetDataX2(), dataGetter->GetDataX3());

			const auto solver = std::make_unique<MathSolver>(x1, x2, x3);
			std::cout << solver->GetAllData() << std::endl;

			Main::PrintDelimiter(std::cout);

			std::cout << std::format("Sum x1: {:.4f}\tSum x1^2: {:.4f}\nSum x2: {:.4f}\tSum x2^2: {:.4f}\nSum x3: {:.4f}\tSum x3^2: {:.4f}\n",
				solver->GetX1Sum(), solver->GetX1SquareSum(), solver->GetX2Sum(),
				solver->GetX2SquareSum(), solver->GetX3Sum(), solver->GetX3SquareSum());

			Main::PrintDelimiter(std::cout);

			std::cout << solver->NormalizedMatrix() << std::endl;

			Main::PrintDelimiter(std::cout);

			std::cout << solver->CorrelationMatrix() << std::endl;

			Main::PrintDelimiter(std::cout);

			std::cout << std::format("Определитель R: {:.4f}\n", solver->CorrelationDeterminant());

			auto [xi_, xit] = std::make_tuple(solver->GetXi(), PearsonTable::GetInstance().GetElement(1.0 - dataGetter->GetFreedom(), 3));

			std::cout << std::format("Значение Xi: {:.4f}\n", xi_);
			std::cout << std::format("Табличное значение Xi: {:.4f}\n", xit);

			Main::PrintDelimiter(std::cout);

			if (xi_ > xit) {
				std::cout << solver->CMatrix() << std::endl;

				Main::PrintDelimiter(std::cout);

				const auto [x1F, x2F, x3F] = solver->GetAllFStatistics();
				const auto f = std::make_unique<FisherF>(15, 2)->GetFStatistics(1.0 - dataGetter->GetFreedom());

				std::cout << std::format("F-критерий x1: {:.4f}\n", x1F);
				std::cout << std::format("F-критерий x2: {:.4f}\n", x2F);
				std::cout << std::format("F-критерий x3: {:.4f}\n", x3F);

				Main::PrintDelimiter(std::cout);

				std::cout << std::format("Табличное значение F: {:.4f}\n", f);

				if (x1F > f && x2F > f && x3F > f) {
					const auto [x1R, x2R, x3R] = solver->GetAllDetermination();

					std::cout << std::format("Коэффициент детерминации x1: {:.4f}\n", x1R);
					std::cout << std::format("Коэффициент детерминации x2: {:.4f}\n", x2R);
					std::cout << std::format("Коэффициент детерминации x3: {:.4f}\n", x3R);

					Main::PrintDelimiter(std::cout);

					std::cout << solver->PartialCorrelationMatrix() << std::endl;

					Main::PrintDelimiter(std::cout);

					std::cout << solver->GetTCriteria() << std::endl;

					Main::PrintDelimiter(std::cout);

				}
			}
		}

		return EXIT_SUCCESS;
	}

private:
	static void SetLocale(unsigned code) {
		#if defined(_WIN32)
			SetConsoleCP(code);
			SetConsoleOutputCP(code);
		#endif
	}

	static void PrintDelimiter(std::ostream& out, const char symbol = '-', const size_t count = 40u) {
		out << std::string(count, symbol) << '\n';
	}

	std::vector<std::string> argv_;
	std::vector<std::string> filePaths_;

	void InitArgv(int argc, char* argv[]) {
		std::copy_n(argv, argc, std::back_inserter(this->argv_));
	}

	void ParseArgs() {
		try {
			const auto argumentParser = std::make_unique<argparse::ArgumentParser>();
			argumentParser->add_argument(Main::FILES_ARG_STR.first, Main::FILES_ARG_STR.second)
				.required()
				.nargs(static_cast<int>(this->argv_.size()) - 2)
				.help("specify the input JSON-files.");

			argumentParser->parse_args(this->argv_);

			this->filePaths_ = argumentParser->get<std::vector<std::string>>(Main::FILES_ARG_STR.first);
		}
		catch (const std::exception&) {
			throw;
		}
	}
};

const std::pair <const char*, const char*> Main::FILES_ARG_STR = { "-f", "--files" };
