#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

void on_trackbar(int, void*)
{
}
int main(int argc, const char *argv[])
{
	
	cv::Mat image;
	
	//image = cv::imread("../Lenna.png", CV_LOAD_IMAGE_COLOR);
	image = cv::imread("../Lenna.png", CV_LOAD_IMAGE_GRAYSCALE);

	//cv::imwrite("out.png", image);
	
	cv::namedWindow("OpenCV", 1);

	int slider_1 = 0;
	cv::createTrackbar("Slider 1", "OpenCV", &slider_1, 10, on_trackbar);

	cv::imshow("OpenCV", image);

	cv::waitKey(0);


	return 0;
}
