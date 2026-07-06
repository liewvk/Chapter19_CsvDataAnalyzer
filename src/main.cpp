#define NOMINMAX

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <limits>
#include <filesystem>

namespace fs = std::filesystem;

struct ExpenseRecord
{
    std::string date;
    std::string category;
    std::string description;
    double amount;
};

struct Summary
{
    int totalRecords = 0;
    double totalAmount = 0.0;
    double averageAmount = 0.0;
    double highestAmount = 0.0;
    double lowestAmount = 0.0;
};

void displayMenu()
{
    std::cout << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "          CSV Data Analyzer" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "1. Load CSV File" << std::endl;
    std::cout << "2. Display All Records" << std::endl;
    std::cout << "3. Show Summary" << std::endl;
    std::cout << "4. Show Category Totals" << std::endl;
    std::cout << "5. Save Analysis Report" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "Please choose an option: ";
}

std::vector<std::string> splitCsvLine(const std::string& line)
{
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;

    while (std::getline(ss, field, ','))
    {
        fields.push_back(field);
    }

    return fields;
}

fs::path findCsvFile(const std::string& filename)
{
    fs::path inputPath = filename;

    if (fs::exists(inputPath))
    {
        return inputPath;
    }

    fs::path current = fs::current_path();

    for (int i = 0; i < 8; i++)
    {
        fs::path candidate1 = current / inputPath;

        if (fs::exists(candidate1))
        {
            return candidate1;
        }

        fs::path candidate2 = current / "data" / inputPath.filename();

        if (fs::exists(candidate2))
        {
            return candidate2;
        }

        if (!current.has_parent_path())
        {
            break;
        }

        current = current.parent_path();
    }

    return {};
}

fs::path findOrCreateDataFolder()
{
    fs::path current = fs::current_path();

    for (int i = 0; i < 8; i++)
    {
        fs::path dataFolder = current / "data";

        if (fs::exists(dataFolder))
        {
            return dataFolder;
        }

        if (!current.has_parent_path())
        {
            break;
        }

        current = current.parent_path();
    }

    fs::path newDataFolder = fs::current_path() / "data";
    fs::create_directories(newDataFolder);

    return newDataFolder;
}

bool loadCsvFile(const std::string& filename, std::vector<ExpenseRecord>& records)
{
    fs::path csvPath = findCsvFile(filename);

    if (csvPath.empty())
    {
        std::cout << "Error: Could not open CSV file." << std::endl;
        std::cout << "Current working directory: " << fs::current_path() << std::endl;
        std::cout << "Please check that expenses.csv is inside the data folder." << std::endl;
        return false;
    }

    std::ifstream file(csvPath);

    if (!file)
    {
        std::cout << "Error: Could not open CSV file." << std::endl;
        return false;
    }

    records.clear();

    std::string line;

    if (!std::getline(file, line))
    {
        std::cout << "Error: CSV file is empty." << std::endl;
        return false;
    }

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::vector<std::string> fields = splitCsvLine(line);

        if (fields.size() != 4)
        {
            std::cout << "Warning: Skipping invalid line: " << line << std::endl;
            continue;
        }

        ExpenseRecord record;
        record.date = fields[0];
        record.category = fields[1];
        record.description = fields[2];

        try
        {
            record.amount = std::stod(fields[3]);
        }
        catch (const std::exception&)
        {
            std::cout << "Warning: Invalid amount. Skipping line: " << line << std::endl;
            continue;
        }

        records.push_back(record);
    }

    file.close();

    std::cout << "CSV file loaded successfully." << std::endl;
    std::cout << "File used: " << csvPath << std::endl;
    std::cout << "Records loaded: " << records.size() << std::endl;

    return true;
}

void displayRecord(const ExpenseRecord& record)
{
    std::cout << "Date       : " << record.date << std::endl;
    std::cout << "Category   : " << record.category << std::endl;
    std::cout << "Description: " << record.description << std::endl;
    std::cout << "Amount     : "
        << std::fixed << std::setprecision(2)
        << record.amount << std::endl;
    std::cout << "------------------------------------" << std::endl;
}

void displayAllRecords(const std::vector<ExpenseRecord>& records)
{
    if (records.empty())
    {
        std::cout << "No records loaded." << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "========== CSV Records ==========" << std::endl;

    for (const ExpenseRecord& record : records)
    {
        displayRecord(record);
    }
}

bool hasLoadedRecords(const std::vector<ExpenseRecord>& records)
{
    if (records.empty())
    {
        std::cout << "No records loaded. Please load a CSV file first." << std::endl;
        return false;
    }

    return true;
}

Summary calculateSummary(const std::vector<ExpenseRecord>& records)
{
    Summary summary;

    if (records.empty())
    {
        return summary;
    }

    summary.totalRecords = static_cast<int>(records.size());
    summary.highestAmount = records[0].amount;
    summary.lowestAmount = records[0].amount;

    for (const ExpenseRecord& record : records)
    {
        summary.totalAmount += record.amount;

        if (record.amount > summary.highestAmount)
        {
            summary.highestAmount = record.amount;
        }

        if (record.amount < summary.lowestAmount)
        {
            summary.lowestAmount = record.amount;
        }
    }

    summary.averageAmount = summary.totalAmount / summary.totalRecords;

    return summary;
}

void displaySummary(const Summary& summary)
{
    std::cout << std::endl;
    std::cout << "========== CSV Summary ==========" << std::endl;
    std::cout << "Total records : " << summary.totalRecords << std::endl;
    std::cout << "Total amount  : "
        << std::fixed << std::setprecision(2)
        << summary.totalAmount << std::endl;
    std::cout << "Average amount: "
        << std::fixed << std::setprecision(2)
        << summary.averageAmount << std::endl;
    std::cout << "Highest amount: "
        << std::fixed << std::setprecision(2)
        << summary.highestAmount << std::endl;
    std::cout << "Lowest amount : "
        << std::fixed << std::setprecision(2)
        << summary.lowestAmount << std::endl;
}

std::map<std::string, double> calculateCategoryTotals(const std::vector<ExpenseRecord>& records)
{
    std::map<std::string, double> categoryTotals;

    for (const ExpenseRecord& record : records)
    {
        categoryTotals[record.category] += record.amount;
    }

    return categoryTotals;
}

void displayCategoryTotals(const std::map<std::string, double>& categoryTotals)
{
    std::cout << std::endl;
    std::cout << "========== Category Totals ==========" << std::endl;

    if (categoryTotals.empty())
    {
        std::cout << "No category totals available." << std::endl;
        return;
    }

    for (const auto& pair : categoryTotals)
    {
        std::cout << std::left << std::setw(12) << pair.first << ": "
            << std::fixed << std::setprecision(2)
            << pair.second << std::endl;
    }
}

void saveAnalysisReport(const std::vector<ExpenseRecord>& records)
{
    if (!hasLoadedRecords(records))
    {
        return;
    }

    Summary summary = calculateSummary(records);
    std::map<std::string, double> categoryTotals = calculateCategoryTotals(records);

    fs::path dataFolder = findOrCreateDataFolder();
    fs::path reportPath = dataFolder / "analysis_report.txt";

    std::ofstream file(reportPath);

    if (!file)
    {
        std::cout << "Error: Could not save analysis report." << std::endl;
        return;
    }

    file << "========== CSV Analysis Report ==========" << std::endl;
    file << "Total records : " << summary.totalRecords << std::endl;
    file << "Total amount  : " << std::fixed << std::setprecision(2)
        << summary.totalAmount << std::endl;
    file << "Average amount: " << std::fixed << std::setprecision(2)
        << summary.averageAmount << std::endl;
    file << "Highest amount: " << std::fixed << std::setprecision(2)
        << summary.highestAmount << std::endl;
    file << "Lowest amount : " << std::fixed << std::setprecision(2)
        << summary.lowestAmount << std::endl;

    file << std::endl;
    file << "========== Category Totals ==========" << std::endl;

    for (const auto& pair : categoryTotals)
    {
        file << std::left << std::setw(12) << pair.first << ": "
            << std::fixed << std::setprecision(2)
            << pair.second << std::endl;
    }

    file.close();

    std::cout << "Analysis report saved successfully." << std::endl;
    std::cout << "Report saved to: " << reportPath << std::endl;
}

int main()
{
    std::vector<ExpenseRecord> records;
    int choice;

    while (true)
    {
        displayMenu();

        if (!(std::cin >> choice))
        {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice)
        {
        case 1:
        {
            std::string filename;

            std::cout << "Enter CSV file name: ";
            std::getline(std::cin, filename);

            if (filename.empty())
            {
                filename = "expenses.csv";
            }

            loadCsvFile(filename, records);
            break;
        }

        case 2:
            displayAllRecords(records);
            break;

        case 3:
            if (hasLoadedRecords(records))
            {
                Summary summary = calculateSummary(records);
                displaySummary(summary);
            }
            break;

        case 4:
            if (hasLoadedRecords(records))
            {
                std::map<std::string, double> categoryTotals =
                    calculateCategoryTotals(records);

                displayCategoryTotals(categoryTotals);
            }
            break;

        case 5:
            saveAnalysisReport(records);
            break;

        case 6:
            std::cout << "Thank you for using the CSV Data Analyzer." << std::endl;
            return 0;

        default:
            std::cout << "Invalid option. Please choose again." << std::endl;
        }
    }
}