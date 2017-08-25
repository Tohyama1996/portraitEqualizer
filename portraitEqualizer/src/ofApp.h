#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Images.h"
#include "Meshes.h"

#define WIDTH 521
#define HEIGHT 734
#define SCALE 1/5

class ofApp : public ofBaseApp{
public:
    void setup();
    void draw();
    void keyPressed(int key);
    void generateImage();
    void setMesh();
    void drawMesh();
    ofColor pickColor(ofPoint _point, vector<ofImage> _images);
    
    //画像配列の設定
    float imageWidth;
    float imageHeight;
    vector<ofImage> images;
    ofxCvHaarFinder finder;
    
    //メッシュの設定
    static const int blockNumX = 30;
    static const int blockNumY = 30;
    int triangleNum = blockNumX*blockNumY*2;
    float blockSizeX;
    float blockSizeY;
    Meshes meshes;
    
    Boolean save = false;
};
