#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/aruco.hpp>
#include <vector>
#include <iostream>
#include "aruco_samples_utility.hpp"

// Считывает парметры детектирования из файла
static bool readDetectorParameters(std::string filename, cv::aruco::DetectorParameters &params) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if(!fs.isOpened())
        return false;

    fs["adaptiveThreshWinSizeMin"] >> params.adaptiveThreshWinSizeMin;
    fs["adaptiveThreshWinSizeMax"] >> params.adaptiveThreshWinSizeMax;
    fs["adaptiveThreshWinSizeStep"] >> params.adaptiveThreshWinSizeStep;
    fs["adaptiveThreshConstant"] >> params.adaptiveThreshConstant;
    fs["minMarkerPerimeterRate"] >> params.minMarkerPerimeterRate;
    fs["maxMarkerPerimeterRate"] >> params.maxMarkerPerimeterRate;
    fs["polygonalApproxAccuracyRate"] >> params.polygonalApproxAccuracyRate;
    fs["minCornerDistanceRate"] >> params.minCornerDistanceRate;
    fs["minDistanceToBorder"] >> params.minDistanceToBorder;
    fs["minMarkerDistanceRate"] >> params.minMarkerDistanceRate;
    fs["cornerRefinementWinSize"] >> params.cornerRefinementWinSize;
    fs["cornerRefinementMaxIterations"] >> params.cornerRefinementMaxIterations;
    fs["cornerRefinementMinAccuracy"] >> params.cornerRefinementMinAccuracy;
    fs["markerBorderBits"] >> params.markerBorderBits;
    fs["perspectiveRemovePixelPerCell"] >> params.perspectiveRemovePixelPerCell;
    fs["perspectiveRemoveIgnoredMarginPerCell"] >> params.perspectiveRemoveIgnoredMarginPerCell;
    fs["maxErroneousBitsInBorderRate"] >> params.maxErroneousBitsInBorderRate;
    fs["minOtsuStdDev"] >> params.minOtsuStdDev;
    fs["errorCorrectionRate"] >> params.errorCorrectionRate;
    return true;
}

void drawCube(cv::Mat& image,int marker_size, const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs, const std::vector<cv::Vec3d>& rvecs, const std::vector<cv::Vec3d>& tvecs) {

    std::vector<cv::Point3d> cube_points;   
    cube_points.push_back(cv::Point3d(marker_size / 2, marker_size / 2, 0));
    cube_points.push_back(cv::Point3d(marker_size / 2, -marker_size / 2, 0));
    cube_points.push_back(cv::Point3d(-marker_size / 2, -marker_size / 2, 0));
    cube_points.push_back(cv::Point3d(-marker_size / 2, marker_size / 2, 0));
    cube_points.push_back(cv::Point3d(marker_size / 2, marker_size / 2, marker_size));
    cube_points.push_back(cv::Point3d(marker_size / 2, -marker_size / 2, marker_size));
    cube_points.push_back(cv::Point3d(-marker_size / 2, -marker_size / 2, marker_size));
    cube_points.push_back(cv::Point3d(-marker_size / 2, marker_size / 2, marker_size));

    std::vector<cv::Point2d> image_points;
    cv::projectPoints(cube_points, rvecs[0], tvecs[0], camera_matrix, dist_coeffs, image_points);
    for (size_t i = 0; i < image_points.size(); i++) { 
        cv::line(image, image_points[0], image_points[1], cv::Scalar(255, 0, 0),     3);
        cv::line(image, image_points[0], image_points[3], cv::Scalar(128, 128, 0),   3);
        cv::line(image, image_points[0], image_points[4], cv::Scalar(0, 255, 0),     3);
        cv::line(image, image_points[4], image_points[5], cv::Scalar(0, 128, 128),   3);
        cv::line(image, image_points[4], image_points[7], cv::Scalar(0, 0, 255),     3);
        cv::line(image, image_points[1], image_points[2], cv::Scalar(200, 0, 200),   3);
        cv::line(image, image_points[1], image_points[5], cv::Scalar(200, 200, 0),   3);
        cv::line(image, image_points[2], image_points[3], cv::Scalar(0, 0, 0),       3);
        cv::line(image, image_points[2], image_points[6], cv::Scalar(0, 200, 200),   3);
        cv::line(image, image_points[3], image_points[7], cv::Scalar(100, 100, 100), 3);
        cv::line(image, image_points[6], image_points[7], cv::Scalar(50, 50, 50),    3);
        cv::line(image, image_points[5], image_points[6], cv::Scalar(200, 150, 150), 3);
    }
}


void projectCubesOnMarkers(cv::Mat &frame, 
                            cv::Ptr<cv::aruco::Dictionary> dictionary,
                            cv::Ptr<cv::aruco::DetectorParameters> params,
                            float marker_length,
                            cv::Mat &camera_matrix,
                            cv::Mat &dist_coeffs,
                            cv::Mat &out) {
    // Копируем оригинальный кадр
    out = frame.clone();

    // Переменные под найденные маркеры
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;

    // Поиск маркеров
    cv::aruco::detectMarkers(frame, dictionary, corners, ids, params);
    
    // Если найден хотя бы один маркер
    if(ids.size() > 0) {
        // Поиск веторов поворота и перемещения для перевода координат маркера в координаты камеры
        std::vector<cv::Vec3d> rvecs, tvecs;
        cv::aruco::estimatePoseSingleMarkers(corners, 50, camera_matrix, dist_coeffs, rvecs, tvecs);
        // Рисуем куб на маркерe
        drawCube(out, 50, camera_matrix, dist_coeffs, rvecs, tvecs);
    }
}

int main(int argc, char *argv[]) {
    // Извлечение данных калибровки камеры
    cv::Mat camera_matrix, dist_coeffs;
    readCameraParameters("../config/calibration.yml", camera_matrix, dist_coeffs);

    // Извлечение параметров детектора
    cv::aruco::DetectorParameters par;
    cv::aruco::DetectorParameters params;

    if(readDetectorParameters("../config/detector_params.yml", par)){
        params = par;
    }

    cv::Ptr<cv::aruco::DetectorParameters> paramsPtr(&params);

    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
    cv::Ptr<cv::aruco::Dictionary> dictionaryPtr(&dictionary);

    cv::VideoCapture video_capture;
    video_capture.open(0);

    int frame_width  = video_capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = video_capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter video("result.avi", cv::VideoWriter::fourcc('M','J','P','G'), 20, cv::Size(frame_width,frame_height));

    // Обработка кадров
    while(1) {
        static int look = 0;
        static cv::Mat out;

        cv::Mat frame;
        video_capture >> frame;
        
        out = frame;

        if(look){
            projectCubesOnMarkers(frame, dictionaryPtr, paramsPtr, 50, camera_matrix, dist_coeffs, frame);
            video.write(frame);

        }

        //Визуализация
        cv::imshow("Video", frame);
        char key = (char) cv::waitKey(1);
        
        if(key == 'e') {
            look = 1;
        }
        else if(key == 'q' || key == 27) 
            break;
    }
} 

