//
// p2.cpp
// Label binary image based on different objects
// Reads a given pgm image, labels it, and saves it to
// another pgm image.
// Created by Dylan Dominguez on 9/26/23.
//

#include "image.h"
#include "DisjSets.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
using std::unordered_map;


using namespace std;
using namespace ComputerVisionProjects;

int
main(int argc, char **argv){

    if (argc!=3) {
        printf("Usage: %s file1 file2\n", argv[0]);
        return 0;
    }
    const string input_file(argv[1]);
    const string output_file(argv[2]);

    Image an_image;
    if (!ReadImage(input_file, &an_image)) {
        cout <<"Can't open file " << input_file << endl;
        return 0;
    }

    //P2
    LabelBinarySequentially(&an_image);

    if (!WriteImage(output_file, an_image)){
        cout << "Can't write to file " << output_file << endl;
        return 0;
    }
}

