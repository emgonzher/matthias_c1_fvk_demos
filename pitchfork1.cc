#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <regex>
#include <limits>
#include <filesystem> // C++17

struct PertData {
    std::vector<double> x, y, u, v, w;
};

PertData read_pert(const std::string &filename) {
    PertData data;
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open " + filename);

    std::string line;
    std::getline(file, line); // skip header

    double c1, c2, c3, c4, c5;
    while (file >> c1 >> c2 >> c3 >> c4 >> c5) {
        data.x.push_back(c1);
        data.y.push_back(c2);
        data.u.push_back(c3);
        data.v.push_back(c4);
        data.w.push_back(c5);
    }
    return data;
}

std::vector<double> read_pressures(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open " + filename);

    std::vector<double> pressures;
    double p;
    while (file >> p) {
        pressures.push_back(p);
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return pressures;
}

std::string extract_number(const std::string &filename) {
    std::regex re("(\\d+)(?=\\.dat$)");
    std::smatch match;
    if (std::regex_search(filename, match, re)) return match[1];
    throw std::runtime_error("No number found in filename: " + filename);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " pert*.dat\n";
        return 1;
    }

    int nr, nphi;
    {
        std::ifstream meta("nr_nphi_line1.txt");
        if (!meta) {
            std::cerr << "Could not open nr_nphi_line1.txt\n";
            return 1;
        }
        std::string line;
        while (std::getline(meta, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            if (!(iss >> nr >> nphi)) {
                std::cerr << "Could not parse nr/nphi from line: " << line << "\n";
                return 1;
            }
            break; // read only first valid line
        }
    }

    std::vector<double> pressures = read_pressures("pressures.dat");
    std::vector<std::pair<double, double>> pwmax;

    std::filesystem::path datadir = std::filesystem::current_path() / "DATA2PLOT";
    std::filesystem::create_directories(datadir);

    double PI = 4.0 * std::atan(1.0);

    try {
        // Collect filenames and sort numerically
        std::vector<std::string> pert_files;
        for (int i = 1; i < argc; ++i)
            pert_files.push_back(argv[i]);

        std::sort(pert_files.begin(), pert_files.end(),
                  [](const std::string &a, const std::string &b) {
                      int na = std::stoi(extract_number(a));
                      int nb = std::stoi(extract_number(b));
                      return na < nb;
                  });

        // Process files in sorted order
        for (size_t i = 0; i < pert_files.size(); ++i) {
            std::string pertfile = pert_files[i];
            std::string num = extract_number(pertfile);

            PertData data = read_pert(pertfile);

            std::vector<double> phi_vals, wrfix_vals;
            phi_vals.reserve(nphi);
            wrfix_vals.reserve(nphi);

            double wmax = std::numeric_limits<double>::lowest();

            for (int j = 0; j < nphi; ++j) {
                int idx = (j + 1) * nr - 1;
                double phi = std::atan2(data.y[idx], data.x[idx]);
                if (phi < 0) phi += 2.0 * PI;
                double wrfix = data.w[idx];
                phi_vals.push_back(phi);
                wrfix_vals.push_back(wrfix);
                if (wrfix > wmax) wmax = wrfix;
            }

            // Save phi_wrfixN.dat
            std::ofstream out(datadir / ("phi_wrfix" + num + ".dat"));
            if (!out) throw std::runtime_error("Could not write phi_wrfix" + num + ".dat");
            for (size_t k = 0; k < phi_vals.size(); ++k)
                out << phi_vals[k] << "\t" << wrfix_vals[k] << "\n";

            if (i < pressures.size())
                pwmax.emplace_back(pressures[i], wmax);
            else
                throw std::runtime_error("Not enough pressures in pressures.dat for file " + pertfile);

            std::cout << "Processed " << pertfile << "\n";
        }

        // Save wmax_results1.dat
        std::ofstream out2(datadir / "wmax_results1.dat");
        if (!out2) throw std::runtime_error("Could not write wmax_results1.dat");
        for (auto &row : pwmax)
            out2 << row.first << "\t" << row.second << "\n";

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}


