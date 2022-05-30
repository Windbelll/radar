#include <map.h>

#define INPUT_SIZE cv::Size(28, 28) //此参数用于调整数字识别网络输入图片分辨率(不建议修改)
#define COLOR_JUDGE_INDEX 80		//此参数用于调整颜色分类中决定性像素数量差(unsigned int)
#define COLOR_THRESHOLD_INDEX 0.35	//此参数用于调整颜色分类及数字识别中二值化阈值(double 0~1)
#define CAP_WIDTH 1920
#define CAP_HEIGHT 1080

using namespace std;

MapRM::MapRM()
{
	cout << "PRTS has been created..." << endl;
}

MapRM::~MapRM()
{
	cout << "PRTS has been destroyed..." << endl;
}

void MapRM::Catoutputs(std::vector<struct Output> &outputsA, std::vector<struct Output> &outputsB, std::vector<struct Output> &outputsC)
{
	for (size_t i = 0; i < outputsA.size(); i++)
		if (outputsA[i].id == 0)
			outputsC.push_back(outputsA[i]);
	for (size_t i = 0; i < outputsB.size(); i++)
		if (outputsB[i].id == 1)
		{
			if ((float)outputsB[i].box.height / (float)outputsB[i].box.width > 1.5 or (float) outputsB[i].box.height / (float)outputsB[i].box.width < 0.5)
				continue;
			outputsC.push_back(outputsB[i]);
		}
}

// void MapRM::UpdateMap(const cv::Mat &src, vector<struct Output> outputs)
// {
// 	for (int j, i = 0; i < outputs.size(); i++)
// 	{
// 		Carinfo tempinfo;
// 		bool flag = 1;
// 		if (outputs[i].id == 1)
// 		{
// 			//对于装甲板的长宽比筛选

// 			//对于装甲板是否有父机器人筛选
// 			for (j = 0; j < outputs.size(); j++)
// 			{
// 				if (outputs[j].id == 0 && (outputs[i].box.x > outputs[j].box.x && outputs[i].box.x < outputs[j].box.x + outputs[j].box.width) && (outputs[i].box.y > outputs[j].box.y && outputs[i].box.y < outputs[j].box.y + outputs[j].box.height))
// 				{
// 					flag = true;
// 					break;
// 				}
// 			}

// 			if (!flag)
// 				continue;
// 			cv::Mat temp = src(outputs[i].box).clone();
// 			cv::Mat tempgary;
// 			int number = 0;

// 			tempgary = SplitBR2Binary(temp, tempinfo);
// 			number = PredictNumber(tempgary);
// 			cout << "[PRTS]get number:" << number << endl;

// 			resize(temp,temp,cv::Size(temp.cols*6,temp.rows*6));
// 			imshow("temp",temp);
// 			// resize(tempgary,tempgary,cv::Size(tempgary.cols*6,tempgary.rows*6));
// 			// imshow("tempgary",tempgary);

// 			tempinfo.car_number = number;
// 			tempinfo.confidence = outputs[j].confidence;
// 			tempinfo.img_position = outputs[j].box;
// 			tempinfo.world_x = tempinfo.world_y = tempinfo.world_z = 0.0;

// 			// vector<Carinfo>::iterator it;
// 			// //如何确定数字？？？(因为数字的不确定性->***)
// 			// for (it = carinfos_.begin(); it != carinfos_.end(); it++)
// 			// {
// 			// 	//如果已经存在此机器人
// 			// 	if ((*it).car_number == number && (*it).color == tempinfo.color)
// 			// 	{
// 			// 		carinfos_.erase(it);
// 			// 		break;
// 			// 	}
// 			// }

// 			carinfos_.push_back(tempinfo);
// 			// cv::waitKey(0);
// 		}
// 	}
// }

void MapRM::UpdateMap(const cv::Mat &src, vector<struct Output> outputs)
{
	for (int i = 0; i < outputs.size(); i++)
	{
		if (outputs[i].id != 0)
			continue;

		Carinfo tempinfo;
		bool replace_signal = false;
		cv::Mat temp = src(outputs[i].box).clone();
		cv::Mat tempgary;

		tempgary = SplitBR2Binary(temp, tempinfo);

		cv::resize(temp, temp, cv::Size(temp.cols, temp.rows));
		imshow("temp", temp);

		tempinfo.age = 30;
		tempinfo.img_position = outputs[i].box;
		tempinfo.world_x = tempinfo.world_y = tempinfo.world_z = 0.0;

		for (size_t i = 0; i < carinfos_.size(); i++)
		{
			if (IsnextTo(tempinfo.img_position, carinfos_[i].img_position) && tempinfo.color == carinfos_[i].color)
			{
				replace_signal = true;
				tempinfo.confidence = carinfos_[i].confidence + 4;
				tempinfo.car_number = carinfos_[i].car_number;
				carinfos_.erase(carinfos_.begin() + i);
				break;
			}
		}
		if (!replace_signal)
		{
			tempinfo.confidence = 30;
			tempinfo.car_number = GetARandomNumber();
		}

		carinfos_.push_back(tempinfo);
		// cv::waitKey(0);
	}
	CheckDatabase();
}

bool IsnextTo(cv::Rect box, cv::Rect box2)
{
	cv::Point centerA(box.x + box.width / 2, box.y + box.height / 2);
	cv::Point centerB(box2.x + box2.width / 2, box2.y + box2.height / 2);
	if (centerA.x - centerB.x < 30 && centerA.x - centerB.x > -30 && centerA.y - centerB.y < 30 && centerA.y - centerB.y > -30)
		return true;
	else
		return false;
}

int MapRM::GetARandomNumber()
{
	bool flag = true;
	for (int i = 1; i < 6; i++)
	{
		flag = true;
		for (size_t j = 0; j < carinfos_.size(); j++)
		{
			if (carinfos_[j].car_number == i)
			{
				flag = false;
				break;
			}
		}
		if (flag)
			return i;
	}
	int min_index = 0;
	for (size_t i = 1; i < carinfos_.size(); i++)
	{
		if (carinfos_[i].confidence < carinfos_[min_index].confidence)
			min_index = i;
	}
	int num;
	num = carinfos_[min_index].car_number;
	carinfos_.erase(carinfos_.begin() + min_index);
	return num;
}

void MapRM::CheckDatabase()
{
	vector<Carinfo>::iterator it;
	for (it = carinfos_.begin(); it != carinfos_.end(); it++)
	{
		(*it).confidence--;
		if ((*it).confidence <= 0)
		{
			carinfos_.erase(it);
			break;
		}
	}
	sort(carinfos_.begin(), carinfos_.end(), compareByConfidence);
}

bool compareByConfidence(Carinfo A, Carinfo B)
{
	if (A.confidence < B.confidence)
		return false;
	return true;
}

void MapRM::Tracker_Init()
{
	tracker_ = cv::TrackerMIL::create();
	cout << "Tracker has been created..." << endl;
}

void MapRM::Numberpredictor_Init(const std::string &model_path)
{
	predictor_ = cv::dnn::readNet(model_path);
	predictor_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	predictor_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

void MapRM::Printinfos()
{
	cout << "[PRTS]\tcarinfos :" << endl;
	cout << "id\tcolor\tposition2d\tposition in world\tconfidence\tage" << endl;
	for (size_t i = 0; i < carinfos_.size(); i++)
	{
		printf("%2d\t %4s\t (%d,%d) \t(%.2f,%.2f,%.2f)\t %2d\t%2d\n", carinfos_[i].car_number, carinfos_[i].color == 1 ? "blue" : "red", carinfos_[i].img_position.x, carinfos_[i].img_position.y, carinfos_[i].world_x, carinfos_[i].world_y, carinfos_[i].world_z, carinfos_[i].confidence, carinfos_[i].age);
	}
}

/**
 * @brief This a function which used to transform BGRimage to Binaryimage
 * 		  Counting the red or blue pixels and remove the channel of BR
 * @param src
 * @param temp store the color infomation
 * @return cv::Mat  type CV_8UC1 value 0 or 255
 */
cv::Mat MapRM::SplitBR2Binary(const cv::Mat &src, Carinfo &temp)
{
	cv::Mat image = src.clone();
	image.convertTo(image, CV_32F, 1.0 / 255.0);
	/*
	kill lights
	*/
	cv::threshold(image, image, COLOR_THRESHOLD_INDEX, 1, cv::THRESH_BINARY);

	int red_score = 0, blue_score = 0;
	for (size_t i = 0; i < image.rows; i++)
	{
		for (size_t j = 0; j < image.cols; j++)
		{
			// white
			if (image.at<cv::Vec3f>(i, j)[2] == 1 && image.at<cv::Vec3f>(i, j)[1] == 1 && image.at<cv::Vec3f>(i, j)[0] == 1)
				continue;
			// blue
			else if (image.at<cv::Vec3f>(i, j)[2] == 1 && image.at<cv::Vec3f>(i, j)[1] == 0 && image.at<cv::Vec3f>(i, j)[0] == 0)
				red_score++;
			else if (image.at<cv::Vec3f>(i, j)[2] == 0 && image.at<cv::Vec3f>(i, j)[1] == 0 && image.at<cv::Vec3f>(i, j)[0] == 1)
				blue_score++;
			for (size_t w = 0; w < 3; w++)
			{
				image.at<cv::Vec3f>(i, j)[w] = 0;
			}
		}
	}

	// for (size_t i = 0; i < image.rows; i++)
	// {
	// 	for (size_t j = 0; j < image.cols; j++)
	// 	{
	// 		printf("%ld %ld %.2f %.2f %.2f\n", i, j, image.at<cv::Vec3f>(i, j)[0], image.at<cv::Vec3f>(i, j)[1], image.at<cv::Vec3f>(i, j)[2]);
	// 	}
	// }

	if (red_score > blue_score + COLOR_JUDGE_INDEX)
	{
		// std::cout << "[PRTS]this car's color is red,redscore:" << red_score << ", bluescore:" << blue_score << std::endl;
		temp.color = 2;
	}
	else if (blue_score > red_score + COLOR_JUDGE_INDEX)
	{
		// std::cout << "[PRTS]this car's color is blue,redscore:" << red_score << ", bluescore:" << blue_score << std::endl;
		temp.color = 1;
	}
	else
	{
		// std::cout << "[PRTS]this car's color is unknown,redscore:" << red_score << ", bluescore:" << blue_score << std::endl;
		temp.color = 0;
	}
	// cv::morphologyEx(image, image, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)));
	cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
	image.convertTo(image, CV_8UC1, 255.0);
	return image;
}

int MapRM::PredictNumber(const cv::Mat &src)
{
	cv::Mat roi = src.clone();
	cv::Mat output_labels, output_stats, output_cens;
	// int num = cv::connectedComponentsWithStats(image, output_labels, output_stats, output_cens);

	// int max = 1;
	// for (size_t i = 1; i < num; i++)
	// {
	// 	if (output_stats.at<int>(i, 4) > output_stats.at<int>(max, 4))
	// 		max = i;
	// }

	// cv::Mat roi = image(cv::Rect(output_stats.at<int>(max, 0), output_stats.at<int>(max, 1), output_stats.at<int>(max, 2), output_stats.at<int>(max, 3))).clone();

	//后续的blobFromImage函数会使图像失真严重，故将其转换为正方形(same as yolo)
	int row = roi.rows;
	int col = roi.cols;
	int maxsize = MAX(row, col);
	cv::Mat input_img = cv::Mat::zeros(maxsize, maxsize, CV_8UC1);
	roi.copyTo(input_img(cv::Rect(0, 0, col, row)));

	cv::Mat input_blob = cv::dnn::blobFromImage(roi, 1.0, INPUT_SIZE);
	predictor_.setInput(input_blob);
	const std::vector<cv::String> &out_names = predictor_.getUnconnectedOutLayersNames();
	cv::Mat out_tensor = predictor_.forward(out_names[0]);
	cv::Point maxLoc;

	cv::minMaxLoc(out_tensor, (double *)0, (double *)0, (cv::Point *)0, &maxLoc);

	cv::resize(roi, roi, cv::Size(roi.cols * 6, roi.rows * 6));
	cv::imshow("roi", roi);

	int answer;
	/*if the answer is a bit lipu*/
	if (maxLoc.x == 7)
		answer = 1;
	else if (maxLoc.x == 8)
		answer = 3;
	else
		answer = maxLoc.x;
	return answer;
}

void MapRM::Location_Init(const std::string &file_name)
{
	ifstream file;
	cout << "start read locations" << endl;
	file.open(file_name);
	int num;
	double data;
	Location_info temp_info;
	while (file >> num)
	{
		if (num == -1)
			break;
		file >> data;
		temp_info.img_position.x = data / 1920;
		file >> data;
		temp_info.img_position.y = data / 1080;
		file >> data;
		temp_info.map_to_red.x = data;
		file >> data;
		temp_info.map_to_red.y = data;
		file >> data;
		temp_info.map_to_blue.x = data;
		file >> data;
		temp_info.map_to_blue.y = data;
		locations_.push_back(temp_info);
	}
	cout << "Locations:" << endl;
	for (size_t i = 0; i < locations_.size(); i++)
	{
		cout << locations_[i].img_position.x << "  " << locations_[i].img_position.y << "  " << locations_[i].map_to_red.x << "  " << locations_[i].map_to_red.y << "  " << locations_[i].map_to_blue.x << "  " << locations_[i].map_to_blue.y << endl;
	}
	cout << "Location completed" << endl;
	file.close();
}

void MapRM::LoadData(int color, vector<Send_msg> &buffer)
{
	Send_msg temp_each;
	int index = 0;
	if (color == 1)
		index = 100;
	for (size_t i = 0; i < carinfos_.size(); i++)
	{
		double x, y, w, h;
		if (carinfos_[i].color == color)
		{
			temp_each.num = index + carinfos_[i].car_number;
			x = (double)carinfos_[i].img_position.x / CAP_WIDTH;
			y = (double)carinfos_[i].img_position.y / CAP_HEIGHT;
			w = (double)carinfos_[i].img_position.width / CAP_WIDTH;
			h = (double)carinfos_[i].img_position.height / CAP_HEIGHT;
			temp_each.map = FindLocation(cv::Point2d(x, y), color);
			cout << "add item:" << temp_each.num << "  " << temp_each.map.x << "  " << temp_each.map.y;
			buffer.push_back(temp_each);
		}
	}
}

cv::Point2d MapRM::FindLocation(cv::Point2d a, int color)
{
	double min_distance = GetDistance(a, locations_[0].img_position);
	int index = 0;
	for (size_t i = 1; i < locations_.size(); i++)
	{
		double d = GetDistance(a, locations_[i].img_position);
		if (d < min_distance)
		{
			min_distance = d;
			index = i;
		}
	}
	if (color == 2)
		return locations_[index].map_to_blue;
	else
		return locations_[index].map_to_red;
}

double GetDistance(cv::Point2d a, cv::Point2d b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void Getmsg(std::vector<Send_msg> &buffer, Send_msg &msg)
{
	std::vector<Send_msg>::iterator it = buffer.begin();
	msg.num = (*it).num;
	msg.map.x = (*it).map.x + random() % 20 / (double)100;
	msg.map.y = (*it).map.y + random() % 20 / (double)100;
	buffer.erase(it);
}