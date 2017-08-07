// Code borrowed heavily from the OpenCV samples

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/aruco/charuco.hpp>

#include <cctype>
#include <iostream>
#include <string.h>
#include <time.h>

#include "CalData.hpp"
#include "ConfigReader.hpp"

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

    fs.release();                                       // explicit close
    cout << "Write Done." << endl;
}


void readCamSettings(FileNode& cameraNode)
{
    CalData* config = CalData::getInstance();
    config->m_camModel.m_width = (int)cameraNode[CAM_WIDTH_TAG];
    config->m_camModel.m_height = (int)cameraNode[CAM_HEIGHT_TAG];
    config->m_camModel.m_fov = (float)cameraNode[CAM_FOV_TAG];
}

void readRendererSettings(FileNode& rendererNode)
{
    CalData* config = CalData::getInstance();
    config->m_previewWidth = (int)rendererNode[RENDERER_PREVIEW_WIDTH_TAG];
    config->m_previewHeight = (int)rendererNode[RENDERER_PREVIEW_HEIGHT_TAG];
    config->m_vertexShaderSourceFile = 
            (string)rendererNode[RENDERER_VERTEX_SHADER_FILENAME_TAG];
    config->m_fragmentShaderSourceFile = 
            (string)rendererNode[RENDERER_FRAGMENT_SHADER_FILENAME_TAG];
    config->m_previewVSSourceFile = 
            (string)rendererNode[RENDERER_PREVIEW_VERTEX_SHADER_FILENAME_TAG];
    config->m_previewFSSourceFile = 
            (string)rendererNode[RENDERER_PREVIEW_FRAGMENT_SHADER_FILENAME_TAG];
}

void readTextureSettings(FileNode& textureNode)
{
    CalData* config = CalData::getInstance();
    int texID = textureNode[TEXTURE_ID_TAG];
    TextureConfigData texture;
    texture.m_type = 
        TextureConfigData::mapStringToTextureType(textureNode[TEXTURE_TYPE_TAG]);
    FileNode charucoNode;
    switch (texture.m_type)
    {
        case TextureType::IMAGE_TEXTURE:
            texture.m_imgFilename = (string)textureNode[TEXTURE_IMAGE_FILENAME_TAG];
            break;
    
        case TextureType::CHARUCO_TEXTURE:
            charucoNode = textureNode[TEXTURE_CHARUCO_TAG];
            texture.m_markerDict = 
                TextureConfigData::mapStringToArUcoDict(charucoNode[CHARUCO_DICT_TAG]);
            texture.m_chessRows = charucoNode[CHARUCO_ROWS_TAG];
            texture.m_chessCols = charucoNode[CHARUCO_COLS_TAG];
            texture.m_chessSqSz = charucoNode[CHESS_SQUARE_SIZE_TAG]; 
            texture.m_arucoSz = charucoNode[ARUCO_SIZE_TAG]; 
            texture.m_pxWidthTarget = charucoNode[CHARUCO_PIXEL_WIDTH_TAG];
            texture.m_pxHeightTarget = charucoNode[CHARUCO_PIXEL_HEIGHT_TAG];
            break;
            
        case TextureType::INVALID:
            cerr << "Invalid texture: " << texID << endl;
            break;
            
        default:
            break;
    }
    
    config->m_textures[texID] = texture;
}

bool readTargetSettings(FileNode& targetNode)
{
    CalData* config = CalData::getInstance();
    bool success = true;
    int targetID = targetNode[TARGET_ID_TAG];
    TargetConfigData target;
    target.m_texID = targetNode[TARGET_TEXTURE_ID_TAG];
    
    FileNode vertexNode = targetNode[TARGET_VERTEX_DATA_TAG];
    if (vertexNode.type() != FileNode::SEQ)
    {
        cerr << "Vertex data is not a sequence in target " << targetID << "!" << endl;
        return false;
    }
    vector<float> vertexArray;
    int vertexCount = 0;
    for (auto node : vertexNode)
    {
        ++vertexCount;
        FileNode posNode = node[VERTEX_POS_TAG];
        vertexArray.push_back(posNode[AXIS_X_TAG]);
        vertexArray.push_back(posNode[AXIS_Y_TAG]);
        vertexArray.push_back(posNode[AXIS_Z_TAG]);
        FileNode texCoordNode = node[VERTEX_TEXTURE_COORDINATE_TAG];
        vertexArray.push_back(texCoordNode[AXIS_X_TAG]);
        vertexArray.push_back(texCoordNode[AXIS_Y_TAG]);
    }
    target.m_vertexData.reset(new float[vertexArray.size()], std::default_delete<float[]>());
    for (size_t ind = 0; ind < vertexArray.size(); ++ind)
    {
        target.m_vertexData.get()[ind] = vertexArray[ind];
    }
    target.m_vertexCount = vertexCount;
    
    FileNode indexNode = targetNode[TRIANGLE_INDICES_TAG];
    if (indexNode.type() != FileNode::SEQ)
    {
        cerr << "Index data is not a sequence in target " << targetID << "!" << endl;
        return false;
    }
    vector<unsigned int> indexArray;
    int triCount = 0;
    for (auto triNode : indexNode)
    {
        ++triCount;
        int triInd = triNode[TRIANGLE_FIRST_INDEX_TAG]; 
        indexArray.push_back(triInd);
        triInd = triNode[TRIANGLE_SECOND_INDEX_TAG];
        indexArray.push_back(triInd);
        triInd = triNode[TRIANGLE_THIRD_INDEX_TAG];
        indexArray.push_back(triInd);
    }
    target.m_indices.reset(new unsigned int[indexArray.size()], 
                                std::default_delete<unsigned int[]>());
    for (size_t ind = 0; ind < indexArray.size(); ++ind)
    {
        target.m_indices.get()[ind] = indexArray[ind];
    }
    target.m_triCount = triCount;
    
    config->m_targets.insert(pair<int, TargetConfigData>(targetID, target));
    
    return true;
}

void readCaptureSettings(FileNode& captureNode)
{
    CalData* config = CalData::getInstance();
    int targetID = captureNode[CAPTURE_TARGET_ID_TAG];
    CapturePositionConfig capture;
    
    // Read transforms
    FileNode tNode = captureNode[CAPTURE_TRANSFORMS_TAG];
    for (auto node : tNode)
    {
        int seq_id = node[CAPTURE_SEQUENCE_ID_TAG];
        GeomTransform transform;
        transform.m_type = 
            GeomTransform::mapStringToTransformType(node[CAPTURE_TRANSFORM_TYPE_TAG]);
        FileNode axisNode = node[TRANSFORM_AXIS_TAG];
        transform.m_axis[0] = axisNode[AXIS_X_TAG];
        transform.m_axis[1] = axisNode[AXIS_Y_TAG];
        transform.m_axis[2] = axisNode[AXIS_Z_TAG];
        transform.m_angle = node[ROTATION_ANGLE_TAG];
        capture.m_transforms[seq_id] = transform;
    }
    
    config->m_captures.push_back(capture);
}

bool readFile(string filename)
{
    bool success = true;

    FileStorage fs;
    fs.open(filename, FileStorage::READ);
    
    FileNode cameraNode = fs[CAMERA_TAG];
    readCamSettings(cameraNode);
    
    FileNode rendererNode = fs[RENDERER_TAG];
    readRendererSettings(rendererNode);

    FileNode textureNode = fs[TEXTURES_TAG];
    if (textureNode.type() != FileNode::SEQ)
    {
        cerr << TEXTURES_TAG << " is not a sequence!" << endl;
        return false;
    }
    for (auto node : textureNode)
    {
        readTextureSettings(node);
    }
    
    FileNode targetNode = fs[TARGETS_TAG];
    if (targetNode.type() != FileNode::SEQ)
    {
        cerr << TARGETS_TAG << " is not a sequence!" << endl;
        return false;
    }
    for (auto node : targetNode)
    {
        success = readTargetSettings(node);
        if (!success) return false;
    }
    
    FileNode captureNode = fs[CAPTURES_TAG];
    if (captureNode.type() != FileNode::SEQ)
    {
        cerr << CAPTURES_TAG << " is not a sequence!" << endl;
        return false;
    }
    for (auto node : captureNode)
    {
        readCaptureSettings(node);
    }
    
    return true;
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
    
    cout << endl;
    
    cout << "Reading: " << endl;
    bool success = readFile(filename);    
    if (!success) return 1;
    
    CalData* config = CalData::getInstance();
    cout << string(*config) << endl;

    return 0;
}
