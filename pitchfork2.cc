#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <stdexcept>
#include <regex>
#include <filesystem>
#include <iomanip>
#include <cmath>
#include <limits>

namespace fs = std::filesystem;

// ---------- Data structures ----------
struct DataFrame {
    std::vector<double> col1, col2, col3, col4, col5; // x, y, u, v, w
};

// ---------- Utilities ----------
static inline double PI() { return 4.0 * std::atan(1.0); }

// Read .dat file (5 columns: x y u v w). Assumes first line is *not* a header.
// If your files include a header line like "ZONE ...", skip it before reading.
DataFrame read_dat(const std::string &filename) {
    DataFrame df;
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open " + filename);

    // If files have a header line, uncomment these two lines:
    // std::string header;
    // std::getline(file, header);

    double c1, c2, c3, c4, c5;
    while (file >> c1 >> c2 >> c3 >> c4 >> c5) {
        df.col1.push_back(c1);
        df.col2.push_back(c2);
        df.col3.push_back(c3);
        df.col4.push_back(c4);
        df.col5.push_back(c5);
    }
    return df;
}

// Extract the last number before ".dat" from filename (e.g., "...42.dat" -> "42")
std::string extract_number(const std::string &filename) {
    std::regex re("(\\d+)(?=\\.dat$)");
    std::smatch match;
    if (std::regex_search(filename, match, re)) return match[1];
    throw std::runtime_error("No number found in filename: " + filename);
}

// Read pressures (one number per line)
std::vector<double> read_pressures(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open " + filename);

    std::vector<double> pressures;
    double p;
    while (file >> p) {
        pressures.push_back(p);
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    if (pressures.empty()) {
        throw std::runtime_error("pressures.dat is empty");
    }
    return pressures;
}

// Read nr and nphi from a file, skipping blank lines and lines starting with '#'
std::pair<int,int> read_nr_nphi_from_line2(const std::string &filename) {
    std::ifstream meta(filename);
    if (!meta) throw std::runtime_error("Could not open " + filename);
    std::string line;
    while (std::getline(meta, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        int nr, nphi;
        if (iss >> nr >> nphi) {
            if (nr <= 0 || nphi <= 0) {
                throw std::runtime_error("Invalid nr/nphi (<=0) in " + filename);
            }
            return {nr, nphi};
        }
    }
    throw std::runtime_error("Failed to parse nr/nphi from " + filename);
}

// Average over each block of nphi points for nr blocks
std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
block_average(int nr, int nphi, const DataFrame &df) {
    const size_t expected = static_cast<size_t>(nr) * static_cast<size_t>(nphi);
    if (df.col3.size() != expected || df.col4.size() != expected || df.col5.size() != expected) {
        throw std::runtime_error("block_average: data size mismatch (have " +
            std::to_string(df.col3.size()) + ", expected " + std::to_string(expected) + ")");
    }

    std::vector<double> u_avg(nr, 0.0), v_avg(nr, 0.0), w_avg(nr, 0.0);

    for (int i_block = 0; i_block < nr; ++i_block) {
        double su = 0.0, sv = 0.0, sw = 0.0;
        const size_t base = static_cast<size_t>(i_block) * static_cast<size_t>(nphi);
        for (int j_point = 0; j_point < nphi; ++j_point) {
            size_t idx = base + static_cast<size_t>(j_point);
            su += df.col3[idx];
            sv += df.col4[idx];
            sw += df.col5[idx];
        }
        u_avg[i_block] = su / nphi;
        v_avg[i_block] = sv / nphi;
        w_avg[i_block] = sw / nphi;
    }
    return {u_avg, v_avg, w_avg};
}

int main(int argc, char *argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " line2_soln*.dat\n";
            return 1;
        }

        // ---- Read metadata (line2) safely ----
        int nr = 0, nphi = 0;
        std::tie(nr, nphi) = read_nr_nphi_from_line2("nr_nphi_line2.txt");

        // ---- Create DATA2PLOT directory ----
        fs::path exe_path = fs::absolute(argv[0]).parent_path();
        fs::path plotdir  = exe_path / "DATA2PLOT";
        if (!fs::exists(plotdir) && !fs::create_directory(plotdir)) {
            throw std::runtime_error("Could not create DATA2PLOT directory");
        }

        // ---- Read pressures once ----
        std::vector<double> pressures = read_pressures("pressures.dat");

        // ---- Collect filenames and sort numerically ----
        std::vector<std::string> datafiles;
        for (int i = 1; i < argc; ++i)
            datafiles.push_back(argv[i]);

        std::sort(datafiles.begin(), datafiles.end(),
                  [](const std::string &a, const std::string &b) {
                      int na = std::stoi(extract_number(a));
                      int nb = std::stoi(extract_number(b));
                      return na < nb;
                  });

        // ---- Will collect (pressure, vector<wmax per block>) for all files ----
        std::vector<std::pair<double, std::vector<double>>> wmax_all;

        // ---- Process files in sorted order ----
        for (size_t i = 0; i < datafiles.size(); ++i) {
            const std::string &datafile = datafiles[i];
            std::string number = extract_number(datafile);

            DataFrame df = read_dat(datafile);
            const size_t expected = static_cast<size_t>(nr) * static_cast<size_t>(nphi);
            if (df.col1.size() != expected || df.col2.size() != expected ||
                df.col3.size() != expected || df.col4.size() != expected ||
                df.col5.size() != expected) {
                throw std::runtime_error("File '" + datafile + "': row count mismatch.");
            }

            auto [u_avg, v_avg, w_avg] = block_average(nr, nphi, df);

            std::vector<double> u_pert(expected), v_pert(expected), w_pert(expected), phi(expected);
            const double twoPI = 2.0 * PI();

            for (int i_block = 0; i_block < nr; ++i_block) {
                size_t base = static_cast<size_t>(i_block) * nphi;
                for (int j = 0; j < nphi; ++j) {
                    size_t idx = base + j;
                    u_pert[idx] = df.col3[idx] - u_avg[i_block];
                    v_pert[idx] = df.col4[idx] - v_avg[i_block];
                    w_pert[idx] = df.col5[idx] - w_avg[i_block];
                    double ph = std::atan2(df.col2[idx], df.col1[idx]);
                    if (ph < 0) ph += twoPI;
                    phi[idx] = ph;
                }
            }

            // ---- Save phi_wN.dat ----
            {
                fs::path outname = plotdir / ("phi_w" + number + ".dat");
                std::ofstream phi_w_file(outname);
                if (!phi_w_file) throw std::runtime_error("Could not write " + outname.string());
                phi_w_file.setf(std::ios::scientific);
                phi_w_file << std::setprecision(8);

                for (int j = 0; j < nphi; ++j) {
                    for (int i_block = 0; i_block < nr; ++i_block) {
                        size_t idx = static_cast<size_t>(i_block) * nphi + j;
                        phi_w_file << phi[idx] << " " << w_pert[idx] << " ";
                    }
                    phi_w_file << "\n";
                }
            }

            // ---- Compute wmax per block ----
            std::vector<double> wmax_block(nr, -std::numeric_limits<double>::infinity());
            for (int i_block = 0; i_block < nr; ++i_block) {
                size_t base = static_cast<size_t>(i_block) * nphi;
                double wmax = w_pert[base];
                for (int j = 1; j < nphi; ++j) {
                    size_t idx = base + j;
                    if (w_pert[idx] > wmax) wmax = w_pert[idx];
                }
                wmax_block[i_block] = wmax;
            }

            // ---- Map sorted file index to pressure ----
            if (i >= pressures.size()) throw std::runtime_error("Not enough pressures for file " + datafile);
            wmax_all.emplace_back(pressures[i], std::move(wmax_block));

            std::cout << "Processed " << datafile << "\n";
        }

        // ---- Save combined wmax_results2.dat ----
        {
            fs::path outname = plotdir / "wmax_results2.dat";
            std::ofstream wmax_file(outname);
            if (!wmax_file) throw std::runtime_error("Could not write " + outname.string());

            wmax_file.setf(std::ios::scientific);
            wmax_file << std::setprecision(8);

            for (const auto &entry : wmax_all) {
                wmax_file << entry.first;
                for (double w : entry.second) wmax_file << " " << w;
                wmax_file << "\n";
            }
        }

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}

