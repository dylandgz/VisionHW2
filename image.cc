// Class for representing a 2D gray-scale image,
// with support for reading/writing pgm images.
// To be used in Computer Vision class.
//
// image.cpp
// Contains functions necessary to run p1,p2,p3 and p4
// Created by Dylan Dominguez on 9/26/23.
//


#include "image.h"
#include "DisjSets.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <unordered_map>

using std::unordered_map;

using namespace std;

namespace ComputerVisionProjects {

    Image::Image(const Image &an_image) {
        pixels_ = nullptr;
        num_rows_ = 0;
        num_columns_ = 0;
        AllocateSpaceAndSetSize(an_image.num_rows(), an_image.num_columns());
        SetNumberGrayLevels(an_image.num_gray_levels());

        for (size_t i = 0; i < num_rows(); ++i)
            for (size_t j = 0; j < num_columns(); ++j) {
                SetPixel(i, j, an_image.GetPixel(i, j));
            }
    }

    Image::~Image() {
        DeallocateSpace();
    }

    void Image::AllocateSpaceAndSetSize(size_t num_rows, size_t num_columns) {
        if (pixels_ != nullptr) DeallocateSpace();
        pixels_ = new int *[num_rows];
        for (size_t i = 0; i < num_rows; ++i)
            pixels_[i] = new int[num_columns];

        num_rows_ = num_rows;
        num_columns_ = num_columns;
    }

    void Image::DeallocateSpace() {
        for (size_t i = 0; i < num_rows_; i++)
            delete pixels_[i];
        delete pixels_;
        pixels_ = nullptr;
        num_rows_ = 0;
        num_columns_ = 0;
    }

    bool ReadImage(const string &filename, Image *an_image) {
        if (an_image == nullptr) abort();
        FILE *input = fopen(filename.c_str(), "rb");
        if (input == 0) {
            cout << "ReadImage: Cannot open file" << endl;
            return false;
        }

        // Check for the right "magic number".
        char line[1024];
        if (fread(line, 1, 3, input) != 3 || strncmp(line, "P5\n", 3)) {
            fclose(input);
            cout << "ReadImage: Expected .pgm file" << endl;
            return false;
        }

        // Skip comments.
        do
            fgets(line, sizeof line, input);
        while (*line == '#');

        // Read the width and height.
        int num_columns, num_rows;
        sscanf(line, "%d %d\n", &num_columns, &num_rows);
        an_image->AllocateSpaceAndSetSize(num_rows, num_columns);


        // Read # of gray levels.
        fgets(line, sizeof line, input);
        int levels;
        sscanf(line, "%d\n", &levels);
        an_image->SetNumberGrayLevels(levels);

        // read pixel row by row.
        for (int i = 0; i < num_rows; ++i) {
            for (int j = 0; j < num_columns; ++j) {
                const int byte = fgetc(input);
                if (byte == EOF) {
                    fclose(input);
                    cout << "ReadImage: short file" << endl;
                    return false;
                }
                an_image->SetPixel(i, j, byte);
            }
        }

        fclose(input);
        return true;
    }

    bool WriteImage(const string &filename, const Image &an_image) {
        FILE *output = fopen(filename.c_str(), "w");
        if (output == 0) {
            cout << "WriteImage: cannot open file" << endl;
            return false;
        }
        const int num_rows = an_image.num_rows();
        const int num_columns = an_image.num_columns();
        const int colors = an_image.num_gray_levels();

        // Write the header.
        fprintf(output, "P5\n"); // Magic number.
        fprintf(output, "#\n");  // Empty comment.
        fprintf(output, "%d %d\n%03d\n", num_columns, num_rows, colors);

        for (int i = 0; i < num_rows; ++i) {
            for (int j = 0; j < num_columns; ++j) {
                const int byte = an_image.GetPixel(i, j);
                if (fputc(byte, output) == EOF) {
                    fclose(output);
                    cout << "WriteImage: could not write" << endl;
                    return false;
                }
            }
        }

        fclose(output);
        return true;
    }

// Implements the Bresenham's incremental midpoint algorithm;
// (adapted from J.D.Foley, A. van Dam, S.K.Feiner, J.F.Hughes
// "Computer Graphics. Principles and practice",
// 2nd ed., 1990, section 3.2.2);
    void DrawLine(int x0, int y0, int x1, int y1, int color,
                  Image *an_image) {
        if (an_image == nullptr) abort();

#ifdef SWAP
#undef SWAP
#endif
#define SWAP(a, b) {a^=b; b^=a; a^=b;}

        const int DIR_X = 0;
        const int DIR_Y = 1;

        // Increments: East, North-East, South, South-East, North.
        int incrE,
                incrNE,
                incrS,
                incrSE,
                incrN;
        int d;         /* the D */
        int x, y;       /* running coordinates */
        int mpCase;    /* midpoint algorithm's case */
        int done;      /* set to 1 when done */

        int xmin = x0;
        int xmax = x1;
        int ymin = y0;
        int ymax = y1;

        int dx = xmax - xmin;
        int dy = ymax - ymin;
        int dir;

        if (dx * dx > dy * dy) {  // Horizontal scan.
            dir = DIR_X;
            if (xmax < xmin) {
                SWAP(xmin, xmax);
                SWAP(ymin, ymax);
            }
            dx = xmax - xmin;
            dy = ymax - ymin;

            if (dy >= 0) {
                mpCase = 1;
                d = 2 * dy - dx;
            } else {
                mpCase = 2;
                d = 2 * dy + dx;
            }

            incrNE = 2 * (dy - dx);
            incrE = 2 * dy;
            incrSE = 2 * (dy + dx);
        } else {// vertical scan.
            dir = DIR_Y;
            if (ymax < ymin) {
                SWAP(xmin, xmax);
                SWAP(ymin, ymax);
            }
            dx = xmax - xmin;
            dy = ymax - ymin;

            if (dx >= 0) {
                mpCase = 1;
                d = 2 * dx - dy;
            } else {
                mpCase = 2;
                d = 2 * dx + dy;
            }

            incrNE = 2 * (dx - dy);
            incrE = 2 * dx;
            incrSE = 2 * (dx + dy);
        }

        /// Start the scan.
        x = xmin;
        y = ymin;
        done = 0;

        while (!done) {
            an_image->SetPixel(x, y, color);

            // Move to the next point.
            switch (dir) {
                case DIR_X:
                    if (x < xmax) {
                        switch (mpCase) {
                            case 1:
                                if (d <= 0) {
                                    d += incrE;
                                    x++;
                                } else {
                                    d += incrNE;
                                    x++;
                                    y++;
                                }
                                break;

                            case 2:
                                if (d <= 0) {
                                    d += incrSE;
                                    x++;
                                    y--;
                                } else {
                                    d += incrE;
                                    x++;
                                }
                                break;
                        }
                    } else {
                        done = 1;
                    }
                    break;

                case DIR_Y:
                    if (y < ymax) {
                        switch (mpCase) {
                            case 1:
                                if (d <= 0) {
                                    d += incrE;
                                    y++;
                                } else {
                                    d += incrNE;
                                    y++;
                                    x++;
                                }
                                break;

                            case 2:
                                if (d <= 0) {
                                    d += incrSE;
                                    y++;
                                    x--;
                                } else {
                                    d += incrE;
                                    y++;
                                }
                                break;
                        } // mpCase
                    } // y < ymin
                    else {
                        done = 1;
                    }
                    break;
            }
        }
    }

//function to convert grey image into binary image using tresholding
    void ConvertToBinary(int treshold, Image *an_image) {
        int total_columns = an_image->num_columns();
        int total_rows = an_image->num_rows();

        for (int y = 0; y < total_rows; y++) {
            for (int x = 0; x < total_columns; x++) {
                if (an_image->GetPixel(y, x) < treshold) {
                    an_image->SetPixel(y, x, 0);
                } else {
                    an_image->SetPixel(y, x, 255);
                }
            }
        }
    }

    void LabelBinarySequentially(Image *an_image) {
        int total_columns = an_image->num_columns();
        int total_rows = an_image->num_rows();
        int greyLvl = 25;//starting grey level
        int curr_pxl;
        int n_pxl, nw_pxl, w_pxl;
        int label = 0;
        unordered_map<int, int> allGreyLvls;
        DisjSets labeling(total_columns * total_rows);
        vector<vector<int>> labels_map(total_columns, vector<int>(total_rows, -1));
        vector<vector<int>> pxl_numbers(total_columns, vector<int>(total_rows, -1));
        unordered_map<int, int> table;

        //first scan
        //will create ans assign sets based on
        //sequential labeling algorithm
        //implements disjoint sets algorithm
        int pxl_counter = 0;
        for (int y = 0; y < total_rows; y++) {
            for (int x = 0; x < total_columns; x++) {
                pxl_numbers[x][y] = pxl_counter;
                pxl_counter++;

                curr_pxl = an_image->GetPixel(y, x);
                //if the pixel value is 0, we ignore the pixel
                if (curr_pxl == 0) {
                    continue;
                } else {
                    //we have to check the edge cases first
                    //if we are in either the first column or row, there will be errors
                    //we have to account for these edge cases
                    if (x == 0 && y == 0) {
                        //treat w_pxl, n_pxl and nw_pxl as not labeled
                        n_pxl = -1;
                        nw_pxl = -1;
                        w_pxl = -1;
                    } else if (y == 0) {
                        //treat n_pxl and nw_pxl as not labeled
                        n_pxl = -1;
                        nw_pxl = -1;
                        w_pxl = labels_map[x - 1][y];
                    } else if (x == 0) {
                        //treat w_pxl and nw_pxl as not labled
                        n_pxl = labels_map[x][y - 1];
                        nw_pxl = -1;
                        w_pxl = -1;
                    } else {
                        n_pxl = labels_map[x][y - 1];
                        nw_pxl = labels_map[x - 1][y - 1];
                        w_pxl = labels_map[x - 1][y];
                    }

                    //we will now assign labels
                    if (n_pxl == -1 && nw_pxl == -1 && w_pxl == -1) {
                        //new label
                        labels_map[x][y] = pxl_numbers[x][y];
                    } else if (nw_pxl != -1) {
                        // current pixel same set label as nw_pxl
                        labels_map[x][y] = pxl_numbers[x][y];
                        labeling.unionSets(labeling.find(pxl_numbers[x - 1][y - 1]), pxl_numbers[x][y]);

                    } else if (n_pxl == -1 && nw_pxl == -1 && w_pxl != -1) {
                        //current pixel set same label as w_pxl
                        labels_map[x][y] = pxl_numbers[x][y];

                        labeling.unionSets(labeling.find(pxl_numbers[x - 1][y]), pxl_numbers[x][y]);

                    } else if (n_pxl != -1 && nw_pxl == -1 && w_pxl == -1) {
                        // current pixel set same label as n_pxl
                        labels_map[x][y] = pxl_numbers[x][y];
                        labeling.unionSets(labeling.find(pxl_numbers[x][y - 1]), pxl_numbers[x][y]);
                    } else if (n_pxl != -1 & nw_pxl == -1 && w_pxl != -1) {
                        //need to see if n_pxl and w_pxl are equal or not, depending on that we will choose an action
                        if (labeling.find(n_pxl) == labeling.find(w_pxl)) {
                            //current pixel same as north
                            labels_map[x][y] = pxl_numbers[x][y];
                            labeling.unionSets(labeling.find(pxl_numbers[x][y - 1]), pxl_numbers[x][y]);
                        } else {
                            //This is the most important part of the code for labeling
                            //this will take care of edge case when the north and west pixels are from different sets
                            //this block of code will join the current pixel, north and west pixel
                            labels_map[x][y] = pxl_numbers[x][y];
                            int set_A = labeling.find(pxl_numbers[x][y - 1]);
                            int set_B = labeling.find(pxl_numbers[x - 1][y]);
                            labeling.unionSets(set_A, pxl_numbers[x][y]);
                            labeling.unionSets(set_A, set_B);


                        }
                    }
                }

            }
        }

        int curr_pxl_label;
        //second pass
        //will loop through all labels that are not -1 to color the pixel the correct color
        //if pixel is -1, it is background, we ignore
        for (int y = 0; y < total_rows; y++) {
            for (int x = 0; x < total_columns; x++) {
                int total_columns = an_image->num_columns();
                int total_rows = an_image->num_rows();
                if (labels_map[x][y] == -1) {
                    continue;
                }
                curr_pxl_label = labeling.find(labels_map[x][y]);

                //if the label is not in the grey lvl hashmap, then add to it
                if (allGreyLvls.find(curr_pxl_label) == allGreyLvls.end()) {
                    allGreyLvls[curr_pxl_label] = greyLvl;
                    greyLvl = greyLvl + 40;
                    an_image->SetPixel(y, x, allGreyLvls[curr_pxl_label]);

                } else {
                    //set the pixel the correct color
                    an_image->SetPixel(y, x, allGreyLvls[curr_pxl_label]);
                }
            }
        }
    }


    void MakeDataset(std::string database_file_path,Image *an_image) {
        int x_max = an_image->num_columns();
        int y_max = an_image->num_rows();
        //create file stream
        ofstream out_stream;
        out_stream.open(database_file_path, std::ofstream::trunc);

        unordered_map<int, int> labeled_areas;
        unordered_map<int, int> x_hat, y_hat;
        unordered_map<int, float> a, a_prime, b, b_prime, c, c_prime;

        //calculating values for center of object
        for (int i = 0; i < y_max; ++i) {
            for (int j = 0; j < x_max; ++j) {
                int curr_pxl_label = an_image->GetPixel(i, j);
                //if the pixel is labeled only, otherwose ignore
                if (curr_pxl_label > 0) {
                    //get area for each label
                    labeled_areas[curr_pxl_label] = labeled_areas[curr_pxl_label] + 1;
                    x_hat[curr_pxl_label] += i;
                    y_hat[curr_pxl_label] += j;
                    a_prime[curr_pxl_label] += pow(i, 2);
                    b_prime[curr_pxl_label] += (i * j);
                    c_prime[curr_pxl_label] += pow(j, 2);

                }
            }
        }

        int label_counter = 1;
        //loop through each object
        for (auto area: labeled_areas) {
            //key value pairs
            int pxl_label = area.first;
            int curr_area = area.second;
            //center of area
            int x_center = (x_hat[pxl_label]) / (double) curr_area;
            int y_center = (y_hat[pxl_label]) / (double) curr_area;
            //notice that b_prime is multiplied by 2
            b_prime[pxl_label] *= 2;
            //calculate values of a, b, c
            a[pxl_label] = a_prime[pxl_label] - pow(x_center, 2) * curr_area;
            b[pxl_label] = b_prime[pxl_label] - 2 * x_center * y_center * curr_area;
            c[pxl_label] = c_prime[pxl_label] - pow(y_center, 2) * curr_area;

            //theta, min moment, max moment
            double theta = atan2(b[pxl_label], a[pxl_label] - c[pxl_label]) / 2;
            double min_moment = a[pxl_label] * pow(sin(theta), 2) - b[pxl_label] * sin(theta) * cos(theta) +
                                c[pxl_label] * pow(cos(theta), 2);
            double theta_2 = theta + (M_PI / 2);
            double max_moment = a[pxl_label] * pow(sin(theta_2), 2) - b[pxl_label] * sin(theta_2) * cos(theta_2) +
                                c[pxl_label] * pow(cos(theta_2), 2);
            float roundedness = min_moment / max_moment;
            //write label and attributes to database
            out_stream << label_counter++ << " ";
            out_stream << x_center << " " << y_center << " " << min_moment << " " << curr_area << " " << roundedness << " " << theta << " "<<"\n\n";
            int hypotenus = 40;
            int x_orientation = x_center + hypotenus * cos(theta);
            int y_orientation = y_center + hypotenus * sin(theta);
            //draw on current image
            DrawLine(x_center, y_center, x_orientation, y_orientation, 250, an_image);
            an_image->SetPixel(x_center, y_center, 250);
        }
        out_stream.close();
    }
//checks if num is within 35% of the ground value
//chose this value based on testing
    bool IsWithin35Percent(double num, double ground) {
        double percent_diff = 0.35 * ground;
        return num >= ground - percent_diff && num <= ground + percent_diff;
    }

    //The CheckObjectFromDatabase function will check for image attributes in the
    //database and will compare against the objects in the current image
    //This will label the detected images by drawing its orientation line
    void CheckObjectFromDatabase(const string database,Image *an_image) {
        int x_max = an_image->num_columns();
        int y_max = an_image->num_rows();
        //create file stream
        ifstream in_stream;
        in_stream.open(database);
        string db_line;
        unordered_map<int, string> database_map;
        int counter = 1;
        // Read line by line from database
        while (getline(in_stream, db_line)) {
            if (db_line == "") {
                continue;
            }
            database_map[counter] = db_line;
            //cout<<db_line<<endl;
            counter++;
        }
        in_stream.close();

        unordered_map<int, int> labeled_areas;
        unordered_map<int, int> x_hat, y_hat;
        unordered_map<int, float> a, a_prime, b, b_prime, c, c_prime;

        //calculating values for center of object
        for (int i = 0; i < y_max; ++i) {
            for (int j = 0; j < x_max; ++j) {
                int curr_pxl_label = an_image->GetPixel(i, j);
                //if the pixel is labeled only
                if (curr_pxl_label > 0) {
                    //get area for each label
                    labeled_areas[curr_pxl_label] = labeled_areas[curr_pxl_label] + 1;
                    x_hat[curr_pxl_label] += i;
                    y_hat[curr_pxl_label] += j;
                    a_prime[curr_pxl_label] += pow(i, 2);
                    b_prime[curr_pxl_label] += (i * j);
                    c_prime[curr_pxl_label] += pow(j, 2);

                }
            }
        }

        int label_counter = 1;
        unordered_map<int, double> image_stats_map;
        //loop through each object in the current image
        for (auto area: labeled_areas) {
            //key value pairs
            int pxl_label = area.first;
            int curr_area = area.second;

            //center of area
            int x_center = (x_hat[pxl_label]) / (double) curr_area;
            int y_center = (y_hat[pxl_label]) / (double) curr_area;

            //notice that b_prime is multiplied by 2
            b_prime[pxl_label] *= 2;

            //calculate values of a, b, c
            a[pxl_label] = a_prime[pxl_label] - pow(x_center, 2) * curr_area;
            b[pxl_label] = b_prime[pxl_label] - 2 * x_center * y_center * curr_area;
            c[pxl_label] = c_prime[pxl_label] - pow(y_center, 2) * curr_area;

            //theta, min moment, max moment
            double theta = atan2(b[pxl_label], a[pxl_label] - c[pxl_label]) / 2;
            double min_moment = a[pxl_label] * pow(sin(theta), 2) - b[pxl_label] * sin(theta) * cos(theta) +
                                c[pxl_label] * pow(cos(theta), 2);
            double theta_2 = theta + (M_PI / 2);
            double max_moment = a[pxl_label] * pow(sin(theta_2), 2) - b[pxl_label] * sin(theta_2) * cos(theta_2) +
                                c[pxl_label] * pow(cos(theta_2), 2);
            float roundedness = min_moment / max_moment;
            int hypotenus = 35;

            int x_orientation = x_center + hypotenus * cos(theta);
            int y_orientation = y_center + hypotenus * sin(theta);
            int stat_index = 0;
            string stat="";

            //fseek(,0,SEEK_SET);
            //getting in formation from the database hashmap
            for (auto pair: database_map) {
                //generating hashmap that holds the stats for object in database
                //iterates through lines in the database

                for (char c_num: pair.second) {
                    if (c_num != ' ') {
                        stat+=c_num;
                    }else{

                        image_stats_map[stat_index] = stod(stat);
                        stat_index++;
                        stat = "";
                    }
                }
                stat_index=0;
                //Will check the images based on minimum area and roundedness
                if (IsWithin35Percent(curr_area, image_stats_map[4]) && IsWithin35Percent(roundedness, image_stats_map[5]))
                {
                    cout<<endl;
                    cout<<"Detected object "<<image_stats_map[0]<<endl;
                    DrawLine(x_center, y_center, x_orientation, y_orientation, 250, an_image);
                }
                image_stats_map.clear();
            }
        }
    }
}  // namespace ComputerVisionProjects







