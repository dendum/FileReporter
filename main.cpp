#include "file_reporter.cpp"

std::vector<fs::path> collectPaths(const fs::path &directory) {
    std::vector<fs::path> file_paths;
    for (const auto &entry: fs::directory_iterator(directory)) {
        if (fs::is_regular_file(entry) || fs::is_directory(entry)) {
            file_paths.push_back(entry.path());
        }
    }
    return file_paths;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>" << std::endl;
        return 1;
    }

    fs::path directory(argv[1]);
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }


    try {
        std::vector<fs::path> file_paths = collectPaths(directory);

        TxtReportFactory txt_factory;
        auto txt_report = txt_factory.createReport();
        txt_report->generateReport(file_paths, "report.txt");

        CsvReportFactory csv_factory;
        auto csv_report = csv_factory.createReport();
        csv_report->generateReport(file_paths, "report.csv");
    }
    catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
