#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
// revision JuiwenHsu@2025-0507-1345: add code init
using namespace std;

struct PPMImage {
    int width, height, maxColorValue;
    vector<unsigned char> data;
};

PPMImage readPPM(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    string magicNumber;
    file >> magicNumber;
    if (magicNumber != "P6") {
        throw runtime_error("Invalid PPM format. Only P6 is supported.");
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
      if (line[0] != '#') break;
    }
    stringstream ss(line);
    ss >> ws;
    ss >>  ws >>  ws;
    ss >> ws;
    ss >> ws;

    file >> ws;
    file >> ws;
    int width, height;
    file >> width >> height;
   
    file >> ws;
    int maxColorValue;
    file >> maxColorValue;
    
    file.get();

    vector<unsigned char> data(width * height * 3);
    file.read(reinterpret_cast<char*>(data.data()), data.size());

    if (file.fail()) {
         throw runtime_error("Error reading image data");
    }

    return {width, height, maxColorValue, data};
}

void writeGrayscalePPM(const string& filename, const PPMImage& image) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Could not open file for writing: " + filename);
    }

    file << "P6\n";
    file << image.width << " " << image.height << "\n";
    file << image.maxColorValue << "\n";

    vector<unsigned char> grayscaleData(image.width * image.height * 3);
    for (int i = 0; i < image.width * image.height; ++i) {
        unsigned char r = image.data[i * 3];
        unsigned char g = image.data[i * 3 + 1];
        unsigned char b = image.data[i * 3 + 2];
        unsigned char gray = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);
        grayscaleData[i * 3] = gray;
        grayscaleData[i * 3 + 1] = gray;
        grayscaleData[i * 3 + 2] = gray;
    }

    file.write(reinterpret_cast<char*>(grayscaleData.data()), grayscaleData.size());
}

int main() {
    try {
        PPMImage image = readPPM("image.ppm");
        writeGrayscalePPM("grayscale_image.ppm", image);
        cout << "Image converted to grayscale successfully." << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}