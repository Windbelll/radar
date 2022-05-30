//*****************************************************************************
//  Copyright 2021 @HDU_PHOENIX
//	Lincense 
//	yolo.h
//	Author windbell
//	This is the head file of yolo net, which used to provide interface of yolo net
//  Time 2021.11.27
//*****************************************************************************
#pragma once
#ifndef _YOLO_H_
#define _YOLO_H_
#include<iostream>
#include<math.h>
#include<opencv2/opencv.hpp>
#include<string>

using std::cout;
using std::endl;

//网络输出的结构体
struct Output {
	//class对应的id [ "car", "armor", "base", "watcher" ]
	int id;
	//结果的置信度
	float confidence;
	//结果的box
	cv::Rect box;
};


class Yolo {
private:
    //本程序通过opencv::dnn模块读取yolov5x6(pytorch)转化而来的onnx实现功能
	cv::dnn::Net mynet;
	//行跨度
	const float netStride[3] = { 8.0, 16.0, 32.0 };
	//先验框(不建议修改)
	const float netAnchors[3][6] = { { 10.0, 13.0, 16.0, 30.0, 33.0, 23.0 },{ 30.0, 61.0, 62.0, 45.0, 59.0, 119.0 },{ 116.0, 90.0, 156.0, 198.0, 373.0, 326.0 } };
	//网络输入大小(yolov5x6为1280*1280,非6版本为640*640,修改会影响精度)
	const cv::Size inputSize=cv::Size(640,640);
	//非极大抑制的iou(交并比大小)
	const float nmsThreshold = 0.2;
	//候选框的置信度筛选
	const float confThreshold = 0.7;
	//类别名
	std::vector<std::string> classNames = { "robot", "armor", "base", "watcher"};

public:
	//默认构造与析构
	Yolo() {
		cout << "CarDetector has been created..." << endl;
	}
	~Yolo() {
		cout << "CarDetector has been destroyed..." << endl;
	}
	//归一化函数(因为yolo4->yolo5所以现在用不到)
	float Sigmoid(float x);
	//读取onnx模型(目前只支持yolo的输出)
	bool ReadONNXmodel(std::string modelPath, bool isCuda=true);
	//识别
	bool Detect(const cv::Mat src,std::vector<Output> &output);
	//根据outputs画预测框
	void DrawPred(cv::Mat &img, const std::vector<Output> &output, cv::Scalar color);
	//打印网络层(测试用)
	void Print_Layers();
};




#endif // !YOLO_H
