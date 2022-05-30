//*****************************************************************************
//  Copyright 2021 @HDU_PHOENIX
//	Lincense
//	yolo.cpp
//	Author windbell
//	This is the source file of yolo net, includes code implementations of interface in yolo.h
//  Time 2021.11.27
//*****************************************************************************
#include "yolo.h"

using namespace cv;
using namespace dnn;
using namespace std;
/**
 * @brief Normalization function
 * @param[in] float x                  input data x
 * @return the float data after normalization
 */
float Yolo::Sigmoid(float x)
{
	return static_cast<float>(1.f / (1.f + exp(-x)));
}
/**
 * @brief Net init function
 * @param[in] std::string modelPath    the file path of ONNX model
 * @param[in] bool isCuda              if cuda use true, else false, the param has been overloaded set to true
 * @return the bool data, return true if read net successfully, else false. If problem happened in model, the function will throw exception
 */
bool Yolo::ReadONNXmodel(std::string modelPath, bool isCuda)
{
	try
	{

		mynet = readNetFromONNX(modelPath);
	}
	catch (const exception &)
	{
		cout << "Read model failed..." << endl;
		return false;
	}
	if (isCuda)
	{
		cout << "Detect device:cuda" << endl;
		mynet.setPreferableTarget(DNN_TARGET_CUDA);
		mynet.setPreferableBackend(DNN_BACKEND_CUDA);
	}
	else
	{
		cout << "Detect device:cpu" << endl;
		mynet.setPreferableTarget(DNN_TARGET_CPU);
		mynet.setPreferableBackend(DNN_BACKEND_DEFAULT);
	}
	cout << "Read model successfully..." << endl;
	return true;
}
/**
 * @brief Net detect function
 * @param[in] const cv::Mat src_image             input image(the type of image can't be CV_8UC4)
 * @param[out] std::vector<Output> &output        outputs of net, stored as struct Output in std::vector
 * @return the bool data, return true if detect successfully, else false.
 */
bool Yolo::Detect(const cv::Mat src_image, std::vector<Output> &output)
{
	// blob为网络输入要求的四维张量
	Mat blob, netInputImg = src_image.clone();

	int col = src_image.cols;
	int row = src_image.rows;
	int maxLen = MAX(col, row);
	//后续的blobFromImage函数会使图像失真严重，故将其转换为正方形
	// Mat resizeImg = Mat::zeros(maxLen, maxLen, CV_8UC3);
	// src_image.copyTo(resizeImg(Rect(0, 0, col, row)));
	// netInputImg = resizeImg;
	//建议使用0，0，0以免影响精度
	blobFromImage(netInputImg, blob, 1 / 255.0, inputSize, cv::Scalar(0, 0, 0), true, false);
	//喂食给网络(´･ω･)ﾉ(._.`)
	//输入层:type: float32[1,3,1280,1280]
	mynet.setInput(blob);
	//输出层:type: float32[1,102000,9]
	std::vector<cv::Mat> netOutputImg;
	mynet.forward(netOutputImg, mynet.getUnconnectedOutLayersNames());

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;

	float ratio_h = (float)netInputImg.rows / inputSize.height;
	float ratio_w = (float)netInputImg.cols / inputSize.width;
	int net_width = classNames.size() + 5; //输出的网络宽度是类别数+5

	float *pdata = (float *)netOutputImg[0].data;
	for (int stride = 0; stride < 3; stride++)
	{ // stride
		int grid_x = (int)(inputSize.width / netStride[stride]);
		int grid_y = (int)(inputSize.height / netStride[stride]);
		for (int anchor = 0; anchor < 3; anchor++)
		{ // anchors
			const float anchor_w = netAnchors[stride][anchor * 2];
			const float anchor_h = netAnchors[stride][anchor * 2 + 1];
			for (int i = 0; i < grid_y; i++)
			{
				for (int j = 0; j < grid_y; j++)
				{
					//获取每一行的box框中含有某个物体的概率
					float box_score = pdata[4];
					if (box_score > confThreshold)
					{
						cv::Mat scores(1, classNames.size(), CV_32FC1, pdata + 5);
						Point classIdPoint;
						double max_class_socre;
						minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
						max_class_socre = (float)max_class_socre;
						if (max_class_socre > confThreshold)
						{
							float x = pdata[0];
							float y = pdata[1];
							float w = pdata[2];
							float h = pdata[3];
							int left = (x - 0.5 * w) * ratio_w;
							int top = (y - 0.5 * h) * ratio_h;
							classIds.push_back(classIdPoint.x);
							confidences.push_back(max_class_socre * box_score);
							boxes.push_back(Rect(left, top, int(w * ratio_w), int(h * ratio_h)));
						}
					}
					pdata += net_width; //指针移到下一行
				}
			}
		}
	}
	//执行非最大抑制以消除具有较低置信度的冗余重叠框
	vector<int> nms_result;
	NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, nms_result);
	for (int i = 0; i < nms_result.size(); i++)
	{
		int idx = nms_result[i];
		Output result;
		result.id = classIds[idx];
		result.confidence = confidences[idx];
		result.box = boxes[idx];
		output.push_back(result);
	}

	if (output.size())
		return true;
	else
		return false;
}
/**
 * @brief Display results function
 * @param[in,out] cv::Mat &img                      input image(source image)
 * @param[in] const std::vector<Output> &output     outputs of net, used to draw rects
 * @param[in] cv::Scalar color                      the color of rects and texts
 * @return void
 */
void Yolo::DrawPred(cv::Mat &img, const std::vector<Output> &output, cv::Scalar color)
{
	for (int i = 0; i < output.size(); i++)
	{
		if (output[i].id != 0 && output[i].id != 1)
			continue;
		int left, top;
		left = output[i].box.x;
		top = output[i].box.y;
		int color_num = i;
		rectangle(img, output[i].box, color, 1, 8);

		string label = classNames[output[i].id] + ":" + to_string(output[i].confidence);
		label = label.substr(0, label.find('.') + 3);
		int baseLine;
		Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
		top = max(top, labelSize.height);
		putText(img, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, color, 1, LINE_AA);
	}
}