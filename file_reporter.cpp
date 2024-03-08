#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::string timeConverter(const fs::path &path) {
    fs::file_time_type file_time = fs::last_write_time(path);
    auto converted_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            file_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t mod_time = std::chrono::system_clock::to_time_t(converted_time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&mod_time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

class FileReport {
public:
    virtual void generateReport(const std::vector<fs::path> &files, const std::string &report_file) = 0;

    virtual ~FileReport() = default;
};

class TxtReport : public FileReport {
public:
    void generateReport(const std::vector<fs::path> &files, const std::string &report_file) override {
        std::ofstream out(report_file);
        if (!out)
            throw std::runtime_error("Could not open report file.\n");

        for (const auto &path: files) {
            out << "Name: " << path.stem().string() << "\n";
            out << "Type: " << (fs::is_directory(path) ? "dir" : path.extension()) << "\n";
            if (fs::is_directory(path))
                out << "Size: " << " " << "\n";
            else
                out << "Size: " << fs::file_size(path) << " bytes\n";

            out << "Last modified: " << timeConverter(path) << "\n";
            out << "---------------\n";
        }
    }
};

class CsvReport : public FileReport {
public:
    void generateReport(const std::vector<fs::path> &files, const std::string &report_file) override {
        std::ofstream out(report_file);
        if (!out)
            throw std::runtime_error("Could not open report file.\n");

        out << "Name,Type,Size,Last modified\n";
        for (const auto &path: files) {
            out << path.stem().string() << ",";
            out << (fs::is_directory(path) ? "dir" : path.extension()) << ",";
            if (fs::is_directory(path))
                out << " ,";
            else
                out << fs::file_size(path) << ",";
            out << timeConverter(path) << "\n";
        }
    }
};

class ReportFactory {
public:
    virtual std::unique_ptr<FileReport> createReport() = 0;

    virtual ~ReportFactory() = default;
};

class TxtReportFactory : public ReportFactory {
public:
    std::unique_ptr<FileReport> createReport() override {
        return std::make_unique<TxtReport>();
    }
};

class CsvReportFactory : public ReportFactory {
public:
    std::unique_ptr<FileReport> createReport() override {
        return std::make_unique<CsvReport>();
    }
};