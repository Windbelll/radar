#pragma once
#ifndef _MAP_H_
#define _MAP_H_

#include <yolo.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <math.h>

struct Carinfo
{
    float world_x, world_y, world_z;
    cv::Rect img_position;
    int car_number;
    int color = 0; // 0 == unknow, 1 == blue, 2 == red
    int age;
    int confidence = 0;
};

struct Location_info
{
    int num;
    cv::Point2d img_position;
    cv::Point2d map_to_red;
    cv::Point2d map_to_blue;
};

struct Send_msg
{
    int num;
    cv::Point2d map;
};

class MapRM
{
private:
    std::vector<Carinfo> carinfos_;
    cv::Ptr<cv::Tracker> tracker_;
    cv::dnn::Net predictor_;
    std::vector<Location_info> locations_;
    double dx = 0, dy = 0;

public:
    MapRM();
    ~MapRM();
    void Location_Init(const std::string &file_name);
    void Tracker_Init();
    void Numberpredictor_Init(const std::string &model_path);
    void Printinfos();
    cv::Mat SplitBR2Binary(const cv::Mat &src, Carinfo &temp);
    void Catoutputs(std::vector<struct Output> &outputsA, std::vector<struct Output> &outputsB, std::vector<struct Output> &outputsC);
    void UpdateMap(const cv::Mat &src, std::vector<struct Output> outputs);
    int GetARandomNumber();
    // double GetDepthFromCamera();
    void LoadData(int color, std::vector<Send_msg>& buffer);
    void CheckDatabase();
    cv::Point2d FindLocation(cv::Point2d a, int color);
    int PredictNumber(const cv::Mat &input_binaryimage);
};
bool IsnextTo(cv::Rect box, cv::Rect box2);
bool compareByConfidence(Carinfo A, Carinfo B);
double GetDistance(cv::Point2d a, cv::Point2d b);
void Getmsg(std::vector<Send_msg> &buffer, Send_msg& msg);
#endif
