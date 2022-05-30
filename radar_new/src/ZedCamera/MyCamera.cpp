#include <MyCamera.h>

MyCamera::MyCamera()
{
}

MyCamera::~MyCamera()
{
    Zed_Camera.close();
    std::cout << "Camera close..." << std::endl;
}

bool MyCamera::open(int mode)
{
    if (mode == 0)
    {
        Init_params.camera_resolution = sl::RESOLUTION::HD1080;
        Init_params.camera_fps = 30;
        Init_params.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
        Init_params.coordinate_units = sl::UNIT::METER;
        Init_params.depth_minimum_distance = 10.0;
        Init_params.depth_maximum_distance = 30.0;
        runtime_parameters.sensing_mode = sl::SENSING_MODE::STANDARD;
        
    }
    // while (!Zed_Camera.isOpened())
    // {
        std::cout<<"Camera is trying to open..."<<std::endl;
        sl::ERROR_CODE err = Zed_Camera.open(Init_params);
        if (err != sl::ERROR_CODE::SUCCESS)
        {
            std::cout << toString(err) << std::endl;
            Zed_Camera.close();
            return false;
        }
    // }

        return true;
}

bool MyCamera::operator>>(cv::Mat &Src_Image)
{
    sl::Mat Src_zed(1920, 1080, sl::MAT_TYPE::U8_C4);
    cv::Mat image_ocv = slMat2cvMat(Src_zed);

    if (Zed_Camera.grab(runtime_parameters) == sl::ERROR_CODE::SUCCESS)
    {
        // std::cout << "get image" << std::endl;
        // A new image is available if grab() returns ERROR_CODE::SUCCESS
        Zed_Camera.retrieveImage(Src_zed, sl::VIEW::LEFT, sl::MEM::CPU);
        // Zed_Camera.retrieveMeasure(depth, MEASURE::DEPTH); // Retrieve depth Mat. Depth is aligned on the left image
        // Zed_Camera.retrieveMeasure(point_cloud, MEASURE::XYZRGBA);
    }
    // Zed_Camera.retrieveImage(Src_zed,sl::VIEW::LEFT,sl::MEM::CPU);
    cv::cvtColor(image_ocv, Src_Image, cv::COLOR_BGRA2BGR);
    return true;
}
