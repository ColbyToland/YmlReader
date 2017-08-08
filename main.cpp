// Code borrowed heavily from the OpenCV samples

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/aruco/charuco.hpp>

#include <cctype>
#include <iostream>
#include <string.h>
#include <time.h>

#include "ConfigParser.hpp"

using namespace cv;
using namespace std;
using namespace epilog;

void writeFile(string filename)    
{
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    size_t stride = 8;
    size_t items = 4;
    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    FileStorage fs(filename, FileStorage::WRITE);
    
    fs << CAMERA_TAG << "{";
        fs << CAM_WIDTH_TAG << 2048;
        fs << CAM_HEIGHT_TAG << 1536;
        fs << CAM_FOV_TAG << 100.0f;
    fs << "}";
    
    fs << RENDERER_TAG << "{";
        fs << RENDERER_PREVIEW_WIDTH_TAG << 1024;
        fs << RENDERER_PREVIEW_HEIGHT_TAG << 768;
        fs << RENDERER_VERTEX_SHADER_FILENAME_TAG << "perspective.vs";
        fs << RENDERER_FRAGMENT_SHADER_FILENAME_TAG << "basic.fs";
        fs << RENDERER_PREVIEW_VERTEX_SHADER_FILENAME_TAG << "preview.vs";
        fs << RENDERER_PREVIEW_FRAGMENT_SHADER_FILENAME_TAG << "preview.fs";
    fs << "}";
    
    fs << TEXTURES_TAG << "[";
        fs << "{";
            fs << TEXTURE_ID_TAG << 1984;
            fs << TEXTURE_TYPE_TAG << "charuco";
            fs << TEXTURE_CHARUCO_TAG << "{";
                fs << CHARUCO_DICT_TAG << "DICT_4X4_1000";
                fs << CHARUCO_ROWS_TAG << 30;
                fs << CHARUCO_COLS_TAG << 15;
                fs << CHARUCO_PIXEL_WIDTH_TAG << 1000;
                fs << CHARUCO_PIXEL_HEIGHT_TAG << 2000;
            fs << "}";
        fs << "}";
    fs << "]";
    
    fs << TARGETS_TAG << "[";
        fs << "{";
            fs << TARGET_ID_TAG << 2017;
            fs << TARGET_TEXTURE_ID_TAG << 1984;
            fs << TARGET_VERTEX_DATA_TAG << "[";
            for (size_t ind = 0; ind < items; ++ind)
            {
                fs << "{";
                    fs << VERTEX_POS_TAG << "{";
                        fs << AXIS_X_TAG << vertices[ind*stride + 0];
                        fs << AXIS_Y_TAG << vertices[ind*stride + 1];
                        fs << AXIS_Z_TAG << vertices[ind*stride + 2];
                    fs << "}";
                    fs << VERTEX_TEXTURE_COORDINATE_TAG << "{";
                        fs << AXIS_X_TAG << vertices[ind*stride + 6];
                        fs << AXIS_Y_TAG << vertices[ind*stride + 7];
                    fs << "}";
                fs << "}";
            }
            fs << "]";
            fs << TRIANGLE_INDICES_TAG << "[";
                fs << "{";
                    fs << TRIANGLE_FIRST_INDEX_TAG << (int)indices[0];
                    fs << TRIANGLE_SECOND_INDEX_TAG << (int)indices[1];
                    fs << TRIANGLE_THIRD_INDEX_TAG << (int)indices[2];
                fs << "}";
                fs << "{";
                    fs << TRIANGLE_FIRST_INDEX_TAG << (int)indices[3];
                    fs << TRIANGLE_SECOND_INDEX_TAG << (int)indices[4];
                    fs << TRIANGLE_THIRD_INDEX_TAG << (int)indices[5];
                fs << "}";
            fs << "]";
        fs << "}";
    fs << "]";
    
    fs << CAPTURES_TAG << "[";
        fs << "{";
            fs << CAPTURE_TARGET_ID_TAG << 2017;
            fs << CAPTURE_TRANSFORMS_TAG << "[";
                fs << "{";
                    fs << CAPTURE_SEQUENCE_ID_TAG << 73;
                    fs << CAPTURE_TRANSFORM_TYPE_TAG << "R";
                    fs << TRANSFORM_AXIS_TAG << "{";
                        fs << AXIS_X_TAG << 1.0f;
                        fs << AXIS_Y_TAG << 0.0f;
                        fs << AXIS_Z_TAG << 0.0f;
                    fs << "}";
                    fs << ROTATION_ANGLE_TAG << 45.0f;
                fs << "}";
                fs << "{";
                    fs << CAPTURE_SEQUENCE_ID_TAG << 157;
                    fs << CAPTURE_TRANSFORM_TYPE_TAG << "R";
                    fs << TRANSFORM_AXIS_TAG << "{";
                        fs << AXIS_X_TAG << 0.0f;
                        fs << AXIS_Y_TAG << 0.0f;
                        fs << AXIS_Z_TAG << -1.0f;
                    fs << "}";
                    fs << ROTATION_ANGLE_TAG << 3.0f;
                fs << "}";
                fs << "{";
                    fs << CAPTURE_SEQUENCE_ID_TAG << 331;
                    fs << CAPTURE_TRANSFORM_TYPE_TAG << "T";
                    fs << TRANSFORM_AXIS_TAG << "{";
                        fs << AXIS_X_TAG << 0.0f;
                        fs << AXIS_Y_TAG << 0.0f;
                        fs << AXIS_Z_TAG << -0.4f;
                    fs << "}";
                fs << "}";
            fs << "]";
        fs << "}";
    fs << "]";

    fs.release();
}
    
int main( int argc, char** argv )
{    
    // Command line argument parsing
    CommandLineParser parser(argc, argv,
        "{help h usage ?|               | print this message        }"
        );
        
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    
    if (!parser.check())
    {
        parser.printMessage();
        parser.printErrors();
        return -1;
    }
    
    string filename = "test.yml";
    
    cout << "Writing: " << endl;
    writeFile(filename);
    cout << "Write Done." << endl;
    
    cout << endl;
    
    cout << "Reading: " << endl;
    bool success = ConfigParser::readFile(filename);    
    if (!success) return 1;
    cout << "Read Done." << endl;
    
    cout << endl;
    
    CalData* config = CalData::getInstance();
    cout << string(*config) << endl;

    return 0;
}
