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

namespace fs = std::filesystem;

// Simple DataFrame-like struct
struct DataFrame {
    std::vector<double> col1, col2, col3, col4, col5;
};

// Read .dat file into DataFrame
DataFrame read_dat(const std::string &filename) {
    DataFrame df;
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open " + filename);

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

// Extract the last number before ".dat" from filename
std::string extract_number(const std::string &filename) {
    std::regex re("(\\d+)(?=\\.dat$)");
    std::smatch match;
    if (std::regex_search(filename, match, re)) {
        return match[1];
    }
    throw std::runtime_error("No number found in filename: " + filename);
}

// Average function like Julia's averaxi
std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
averaxi(int nr, int nphi, const DataFrame &df) {
    std::vector<double> u(nr, 0.0), v(nr, 0.0), w(nr, 0.0);
    for (int i = 0; i < nphi; ++i) {
        for (int j = 0; j < nr; ++j) {
            u[j] += df.col3[i * nr + j];
            v[j] += df.col4[i * nr + j];
            w[j] += df.col5[i * nr + j];
        }
    }
    for (int j = 0; j < nr; ++j) {
        u[j] /= nphi;
        v[j] /= nphi;
        w[j] /= nphi;
    }
    return {u, v, w};
}

// Repeat a vector nphi times (column expansion)
std::vector<double> repeat_vector(const std::vector<double> &vec, int nphi) {
    std::vector<double> result;
    result.reserve(vec.size() * nphi);
    for (int i = 0; i < nphi; ++i) {
        result.insert(result.end(), vec.begin(), vec.end());
    }
    return result;
}

// Save data with header
void save_dat(const std::filesystem::path &filename,
              const std::vector<double> &c1,
              const std::vector<double> &c2,
              const std::vector<double> &c3,
              const std::vector<double> &c4,
              const std::vector<double> &c5,
              int nr, int nphi) {
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("Could not write " + filename.string());

    // Optional: Tecplot variable names (some readers require them)
    // file << "VARIABLES = \"X\" \"Y\" \"U\" \"V\" \"W\"" << std::endl;

    // Match Julia spacing exactly: note spaces around comma and trailing space
    file << "ZONE I=" << nr << " , J=" << nphi << " " << std::endl;

    // Use scientific notation with consistent precision
    file.setf(std::ios::scientific);
    file << std::setprecision(8);

    const size_t n = c1.size();
    for (size_t i = 0; i < n; ++i) {
        file << c1[i] << " "
             << c2[i] << " "
             << c3[i] << " "
             << c4[i] << " "
             << c5[i] << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " data*.dat\n";
        return 1;
    }

    // Read metadata once
	int nr = 0, nphi = 0;
	{
		std::ifstream meta("nr_nphi_line1.txt");
		if (!meta) {
		    std::cerr << "Could not open nr_nphi_line1.txt\n";
		    return 1;
		}

		std::string line;
		while (std::getline(meta, line)) {
		    // Skip empty lines and lines starting with #
		    if (line.empty() || line[0] == '#') continue;

		    std::istringstream iss(line);
		    if (!(iss >> nr >> nphi)) {
		        std::cerr << "Could not parse nr/nphi from line: " << line << "\n";
		        return 1;
		    }
		    break; // stop after reading first valid line
		}
	}


    // Output directory management
    fs::path outdir = "PPDAT";
	if (fs::exists(outdir)) {
	    std::cout << "Directory '" << outdir << "' already exists. "
		      << "Do you want to clear its contents? (y/n): ";
	    char choice;
	    std::cin >> choice;
	    if (choice == 'y' || choice == 'Y') {
		for (const auto &entry : fs::directory_iterator(outdir)) {
		    fs::remove_all(entry.path());
		}
		std::cout << "Directory cleared.\n";
	    } else {
		std::cout << "Please move or rename the existing directory '"
			  << outdir << "' and run the program again.\n";
		return 0; // Exit program without doing anything else
	    }
	} else {
	    if (!fs::create_directory(outdir)) {
		std::cerr << "Could not create directory: " << outdir << "\n";
		return 1;
	    }
	}

    try {
        for (int argi = 1; argi < argc; ++argi) {
            std::string datafile = argv[argi];
            std::string number = extract_number(datafile);

            DataFrame df = read_dat(datafile);

            // Save fullN.dat
            save_dat(outdir / ("full" + number + ".dat"),
                     df.col1, df.col2, df.col3, df.col4, df.col5, nr, nphi);

            // Compute averages
            auto [u, v, w] = averaxi(nr, nphi, df);
            auto u_expand = repeat_vector(u, nphi);
            auto v_expand = repeat_vector(v, nphi);
            auto w_expand = repeat_vector(w, nphi);

            // Save averagedN.dat
            save_dat(outdir / ("averaged" + number + ".dat"),
                     df.col1, df.col2, u_expand, v_expand, w_expand, nr, nphi);

            // Compute perturbations
            std::vector<double> u_pert, v_pert, w_pert;
            u_pert.reserve(df.col3.size());
            v_pert.reserve(df.col4.size());
            w_pert.reserve(df.col5.size());
            for (size_t i = 0; i < df.col3.size(); ++i) {
                u_pert.push_back(df.col3[i] - u_expand[i]);
                v_pert.push_back(df.col4[i] - v_expand[i]);
                w_pert.push_back(df.col5[i] - w_expand[i]);
            }

            // Save pertN.dat
            save_dat(outdir / ("pert" + number + ".dat"),
                     df.col1, df.col2, u_pert, v_pert, w_pert, nr, nphi);

            std::cout << "Processed " << datafile << "\n";
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

