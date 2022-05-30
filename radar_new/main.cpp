#define OUR_COLOR 2  // BLUE 2 or RED 1

#include <yolo.h>
#include <iostream>
#include <map.h>

#include <serial_device.h>
#include <com_judge.h>
#include <hardware_interface.h>
#include <thread>
#include <chrono>
#include <mindvision/mindvision.h>

void Thread_JudgeCommunication();
void Thread_PredictSystem();
void GetTime_ms();
std::chrono::milliseconds start_time, main_time; //时间轴(ms)
using std::chrono_literals::operator""ms;

int msg_seq = 0; //发送的数据包序号

/*The Eyes of PRTS*/
MapRM prts;
/*Yolov5 CarDetector*/
Yolo net;
Yolo car_detector;
int main()
{
    car_detector.ReadONNXmodel("./models/cardetectorS.onnx");
    // net.ReadONNXmodel("./models/best.onnx");
    prts.Location_Init("./models/location_in_img.txt");
    start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::thread thA(Thread_JudgeCommunication);
    std::thread thB(Thread_PredictSystem);
    thA.join();
    thB.join();
}
//此处等待测试，10hz的具体检查形式，并据此考虑发送数据的加载方式
void Thread_JudgeCommunication()
{
    JudgeSerial Judge;
    Judge.Init();
    GetTime_ms();
    std::vector<Send_msg> buffer;
    Send_msg msg;
    msg.num = 1;
    msg.map.x = msg.map.y = 1;
    while (true)
    {
        if (buffer.empty())
        {
            prts.LoadData(OUR_COLOR, buffer);
        }
        if (!buffer.empty())
        {
            Getmsg(buffer, msg);
        }
        Radar_Send2MiniMap(msg.num, msg.map.x, msg.map.y, Judge);
        std::this_thread::sleep_for(100ms);
        GetTime_ms();
        printf("[PRTS]Try to send message: carnumber = %d, locate on (%.2f ,%.2f)", msg.num, msg.map.x, msg.map.y);
    }
}
/*
Main Program
*/
void Thread_PredictSystem()
{
    cv::VideoCapture camera("/home/radar/Desktop/3.avi"); // test :"/home/radar/Desktop/3.avi"
    if (camera.isOpened())
    {
        camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        camera.set(cv::CAP_PROP_FPS, 15);
        camera.set(cv::CAP_PROP_FRAME_WIDTH, 2592);  // max 2592
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1944); // max 1944  convert to 16:9 1458
    }

    cout << "FPS: " << camera.get(cv::CAP_PROP_FPS) << endl;

    while (camera.isOpened())
    {
        cv::Mat src1;
        camera >> src1;
        // src1 = src1(cv::Rect(0,293,src1.cols,1458));
        std::vector<struct Output> robot_outputs;
        cv::Mat src1_clone = src1.clone();
        car_detector.Detect(src1, robot_outputs);

        car_detector.DrawPred(src1_clone, robot_outputs, cv::Scalar(0, 255, 255));
        cv::imshow("net", src1_clone);
        prts.UpdateMap(src1, robot_outputs);
        cv::waitKey(5);
    }
}
/**
 * @brief 该函数用于捕获自程序启动开始的运行时间(ms)
 *
 */
void GetTime_ms()
{
    main_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    int64_t dtime = main_time.count() - start_time.count();
    cout << "[Time]" << dtime / 1000 / 60 << ":" << (dtime / 1000) % 60 << ":" << dtime % 1000 << endl;
}