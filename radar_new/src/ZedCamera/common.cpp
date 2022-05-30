#include<common.h>

// float map_x=0;
// float map_y=0;


// cv::Mat slMat2cvMat(sl::Mat input) {
//     // Mapping between MAT_TYPE and CV_TYPE
//     int cv_type = -1;
//     switch (input.getDataType()) {
//         case sl::MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
//         case sl::MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
//         case sl::MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
//         case sl::MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
//         case sl::MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
//         case sl::MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
//         case sl::MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
//         case sl::MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
//         default: break;
//     }
//     return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(sl::MEM::CPU));
// }


// Get the names of the output layers
// std::vector<std::string> getOutputsNames(const cv::dnn::Net& net) {
//     static std::vector<std::string> names;
//     if (names.empty()) {
//         // Get the indices of the output layers, i.e. the layers with
//         // unconnected outputs
//         std::vector<int> outLayers = net.getUnconnectedOutLayers();
//
//         // get the names of all the layers in the network
//         std::vector<std::string> layersNames = net.getLayerNames();
//
//         // Get the names of the output layers in names
//         names.resize(outLayers.size());
//         for (size_t i = 0; i < outLayers.size(); ++i)
//             names[i] = layersNames[outLayers[i] - 1];
//     }
//     return names;
// }
//
// get object's depth 


// void getDepth(int left, int top, int right, int bottom,sl::Mat& point_cloud,cv::Mat &frame){
//     // Get and print distance value in m at the center of the image
//     // We measure the distance camera - object using Euclidean distance
//     int x=(left+right)/2;
//     int y=(top+bottom)/2;

//     float mean_x=0;
//     float mean_y=0;
//     float mean_z=0;

//     sl::float4 point_cloud_value;
//     point_cloud.getValue(x, y, &point_cloud_value);

//     if(std::isfinite(point_cloud_value.z)){
//         float distance = sqrt(point_cloud_value.x * point_cloud_value.x + point_cloud_value.y * point_cloud_value.y + point_cloud_value.z * point_cloud_value.z);
//         std::cout<<"Distance to Camera at {"<<x<<";"<<y<<"}: "<<distance<<"m"<<std::endl;
//         std::cout<<"point_cloud:"<<" x:"<<point_cloud_value.x<<"  y:"<<point_cloud_value.y<<"  z:"<<point_cloud_value.z<<std::endl;
//     }else
//         std::cout<<"The Distance can not be computed at {"<<x<<";"<<y<<"}"<<std::endl;    

//     int k=6;
//     for(int i=-k;i<k;i++)
//     {
//         for(int j=-k;j<k;j++)
//         {
//             point_cloud.getValue(x+i, y+j, &point_cloud_value);
//             mean_x+=point_cloud_value.x;
//             mean_y+=point_cloud_value.y;
//             mean_z+=point_cloud_value.z;
//         }
//     }

//     mean_x/=4*k*k;
//     mean_y/=4*k*k;
//     mean_z/=4*k*k;

//     float dis = sqrt(mean_x*mean_x+mean_y*mean_y+mean_z*mean_z);
//     std::string label = cv::format("X:%.2f Y:%.2f Z:%.2f Dis:%.2f",mean_x,mean_y,mean_z,dis);
//     //地图参数
//     map_x=mean_x/4.0*400;
//     map_y=mean_z/10.0*800;

//     putText(frame,label,cv::Point(left,top-10),cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255),1);       

// }