#include "ofApp.h"

void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);
    ofSetWindowShape(WIDTH, HEIGHT);
    
    cout << "0" << endl;
    finder.setup("haarcascade_frontalface_default.xml");  //顔認識の学習データを読み込む
    cout << "1" << endl;

    generateImage();  //読み込んだ画像から顔部分を判別し、切り取り、画面サイズにリサイズして配列に挿入
    cout << "2" << endl;
    setMesh(); //画像をモザイク調にし、頂点と色データを配列に挿入
    cout << "3" << endl;

}

void ofApp::draw() {
    ofBackground(0);
    
    //saveがtrueならpdf書き出しを開始
    if (save) ofBeginSaveScreenAsPDF("exportedData/exported-"+ ofToString(ofGetDay()) + ofToString(ofGetHours()) + ofToString(ofGetMinutes()) + ofToString(ofGetSeconds()) +".pdf", false);
    
    //描画
    drawMesh();
    
    //pdf書き出しを終了
    if (save) {
        ofEndSaveScreenAsPDF();
        save = false;
    }
    
    ofDrawBitmapStringHighlight("Press 's' key to save.", 20, 20);
}

void ofApp::keyPressed(int key){
    if (key == 's') save = true;
}

void ofApp::generateImage(){
    float mag = 1.5;
    
    int pictureNum = 0;
    for (int i=0; i<19; i++) {
        ofImage _inputImage = ofImage();
        _inputImage.load("images/" + ofToString(pictureNum) + ".jpg"); //bin/data/images/から画像データを読み込み
        finder.findHaarObjects(_inputImage);  //画像から顔部分を認識
        ofRectangle _cur = finder.blobs[0].boundingRect;  //顔部分の矩形を取得
        ofImage _outputImage = ofImage();
        _outputImage.allocate((int)_cur.width*mag, (int)_cur.height*mag, OF_IMAGE_COLOR);
        
        for (int y=0; y < (int)_cur.height*mag-1; y++) {
            for (int x=0; x < (int)_cur.width*mag-1; x++) {
                int _insertX = x;
                int _insertY = y;
                int _extractX = x + _cur.x - _cur.width*((mag-1.0)/2);
                if (_extractX < 0) _extractX = 0;
                else if (_extractX > _inputImage.getWidth()-1) _extractX = _inputImage.getWidth()-1;
                int _extractY = y + _cur.y - _cur.height*((mag-1.0)/2);
                if (_extractY < 0) _extractY = 0;
                else if (_extractY > _inputImage.getHeight()-1) _extractY = _inputImage.getHeight()-1;
                _outputImage.setColor(_insertX, _insertY, _inputImage.getColor(_extractX, _extractY));  //顔部分のピクセルのみをコピー
            }
        }
        
        imageWidth = ofGetHeight();
        imageHeight = ofGetHeight();
        _outputImage.resize(imageWidth, imageHeight);  //リサイズ
        images.push_back(_outputImage);  //配列に挿入
        pictureNum++;
    }
    
}

void ofApp::setMesh() {
    //分割数からメッシュのサイズを計算
    blockSizeX = imageWidth / blockNumX;
    blockSizeY = imageHeight / blockNumY;
    meshes = Meshes();
    
    for (int j=0; j<blockNumY; j++) {
        for (int k=0; k<blockNumX; k++) {
            
            //三角形のメッシュの頂点位置を計算
            ofPoint _point0 = ofPoint(k*blockSizeX, j*blockSizeY);
            ofPoint _point1 = ofPoint(k*blockSizeX, j*blockSizeY+blockSizeY);
            ofPoint _point2 = ofPoint(k*blockSizeX+blockSizeX, j*blockSizeY);
            ofPoint _point3 = ofPoint(k*blockSizeX+blockSizeX, j*blockSizeY+blockSizeY);
            
            //メッシュの重心の位置を計算、重心の色を画像から取得
            ofPoint _center = (_point0+_point1+_point2)/3;
            ofColor _color = ofColor(pickColor(_center, images));
            meshes.add(_point0, _point1, _point2, _color);
            
            //メッシュの重心の位置を計算、重心の色を画像から取得
            _center = (_point1+_point2+_point3)/3;
            _color = ofColor(pickColor(_center, images));
            meshes.add(_point1, _point2, _point3, _color);
        }
    }
    
}

void ofApp::drawMesh() {
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2-imageWidth/2, ofGetHeight()/2-imageHeight/2);
    
    //配列からメッシュの情報を呼び出して描画
    for (int j=0; j<triangleNum; j++) {
        ofFill();
        ofSetColor(meshes.colors[j]);
        ofDrawTriangle(meshes.triangles[j][0], meshes.triangles[j][1], meshes.triangles[j][2]);
    }
    
    ofPopMatrix();
}

ofColor ofApp::pickColor(ofPoint _point, vector<ofImage> _images) {
    unsigned int RSum;
    unsigned int GSum;
    unsigned int BSum;
    ofVec3f _colorSum;
    ofColor _averageColor;
    
    //配列内の画像から同位置の色を取得して平均値を取得
    for (int i=0; i<_images.size(); i++) {
        ofColor _color = _images[i].getColor(_point.x, _point.y);
        ofVec3f _colorVec(_color.r, _color.g, _color.b);
        _colorSum += _colorVec;
    }
    
    _colorSum /= _images.size();
    _averageColor.r = _colorSum.x;
    _averageColor.g = _colorSum.y;
    _averageColor.b = _colorSum.z;
    
    //色の平均値を返す
    return _averageColor;
}
