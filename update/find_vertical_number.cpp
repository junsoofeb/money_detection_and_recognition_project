#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int index = 0;
int money = 0;
char direct = 0;
int j;

char dir_name[100];
vector<String> filenames;
Mat last_roi;


typedef struct 
{
	int max_x = -1;
	int min_x = -1;
	int max_y = -1;
	int min_y = -1;
}min_max_points;



void show(Mat img)
{
	imshow("", img);
	waitKey(0);
	destroyAllWindows();
}



Mat make_hsv_img(Mat frame)
{
	Mat blur, hsv, mask;
	GaussianBlur(frame, blur, Size(3, 3), 0);
	cvtColor(blur, hsv, COLOR_RGB2HSV);
	
	unsigned char lower[3] = {108, 23, 82};
	unsigned char upper[3] = {179, 255, 255};

	Mat lower_color(1, 3, CV_8U, lower);
	Mat upper_color(1, 3, CV_8U, upper);
	
	inRange(hsv, lower_color, upper_color, mask);
	medianBlur(mask, blur, 5);


	// 10 X 10 ���� Ŀ�η� ���� ũ���� ������ ����
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
	morphologyEx(blur, hsv, MORPH_CLOSE, kernel);

	//show(hsv);

	return hsv;
}


int find_hand(Mat hsv_img, Mat origin)
{

	Mat temp = hsv_img.clone();
	Mat ori = origin.clone();

	// ���� �ڸ� �̹����ȿ� �ִ��� üũ 
	int hand_check = 0;

	//������ ã�� minAeraRect����
	vector< vector<Point> > contour, contour_roi;

	findContours(hsv_img, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	vector<Rect> boundRect(contour.size());
	vector<RotatedRect> minRect(contour.size());

	// minAreaRect ����
	for (int i = 0; i < contour.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contour[i]));
	}

	// minAreaRect �׸���
	for (int i = 0; i < contour.size(); i++)
	{
		int area = contourArea(contour[i]);
		//cout << area << endl;
		// 8000�� test �̹��� 10������ ���� 
		if (area > 8000)
		{
			// draw��� ������ �ּ� ���� 
			Point2f rect_points[4];
			minRect[i].points(rect_points);
			drawContours(ori, contour, i, (0, 0, 255), 2, 8);
			hand_check = 1;
		}
	}

	//show(ori);


	// �ڸ� �̹��� �ȿ� �� ������
	if (hand_check == 1)
		return 1;

	// ������,
	else
		return 0;
}


void vertical_number(Mat img)
{

	Mat copy = img.clone();
	Mat gray, canny, blur,result;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, blur, Size(5, 5), 0);
	Canny(blur, canny, 50, 150);

	//show(canny);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 1));
	morphologyEx(canny, result, MORPH_ERODE, kernel);
	
	//show(result);

	Mat kernel2 = getStructuringElement(MORPH_RECT, Size(4, 6));
	morphologyEx(result, result, MORPH_DILATE, kernel2);

	//show(result);


	//������ ã�� minAeraRect����
	vector< vector<Point> > contour, contour_roi;

	findContours(result, contour, RETR_LIST, CHAIN_APPROX_NONE);

	vector<Rect> boundRect(contour.size());
	vector<RotatedRect> minRect(contour.size());

	// minAreaRect ����
	for (int i = 0; i < contour.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contour[i]));
	}

	// �ĺ��� ����� ī��Ʈ
	int count = 0;

	// minAreaRect �׸���
	for (int i = 0; i < contour.size(); i++)
	{
		Point2f rect_points[4];
		minRect[i].points(rect_points);


		int max_x = rect_points[0].x;
		int min_x = rect_points[0].x;
		int max_y = rect_points[0].y;
		int min_y = rect_points[0].y;

		for (int j = 1; j < 4; j++)
		{
			if (max_x < rect_points[j].x)
				max_x = rect_points[j].x;

			if (rect_points[j].x < min_x)
				min_x = rect_points[j].x;

			if (max_y < rect_points[j].y)
				max_y = rect_points[j].y;

			if (rect_points[j].y < min_y)
				min_y = rect_points[j].y;
		}


		int garo = max_x - min_x;
		int sero = max_y - min_y;
		int area = (max_x - min_x) * (max_y - min_y);
		int ratio = 0;
		if (garo == 0 || sero == 0)
			ratio = 0;
		else
			ratio = garo / sero;

		int C_area = contourArea(contour[i]);

		// ���κ��� ���ΰ� �� ���, ũ�Ⱑ ���� �̻� �Ÿ���
		// �׽�Ʈ ��� 700 ~ 1000 ���� 
		if (sero > garo && C_area > 600)
			count++;
	}



	//////////////////////////////////
	cout << "�ĺ��� ���� >> " << count << endl;
	if (count > 20)
	{
		cout << "�ĺ��� 20���� �ѽ��ϴ�." << endl;
		return;
	}
	min_max_points candidate[20];
	int index = 0;
	//////////////////////////////////


	// �ִ� �ּ� x,y ��ǥ ���� �뵵

	for (int i = 0; i < contour.size(); i++)
	{
		Point2f rect_points[4];
		minRect[i].points(rect_points);


		int max_x = rect_points[0].x;
		int min_x = rect_points[0].x;
		int max_y = rect_points[0].y;
		int min_y = rect_points[0].y;

		for (int j = 1; j < 4; j++)
		{
			if (max_x < rect_points[j].x)
				max_x = rect_points[j].x;

			if (rect_points[j].x < min_x)
				min_x = rect_points[j].x;

			if (max_y < rect_points[j].y)
				max_y = rect_points[j].y;

			if (rect_points[j].y < min_y)
				min_y = rect_points[j].y;
		}


		int garo = max_x - min_x;
		int sero = max_y - min_y;
		int area = (max_x - min_x) * (max_y - min_y);
		int ratio = 0;
		if (garo == 0 || sero == 0)
			ratio = 0;
		else
			ratio = garo / sero;

		int C_area = contourArea(contour[i]);

		// ���κ��� ���ΰ� �� ���, ũ�Ⱑ ���� �̻� �Ÿ���
		// �׽�Ʈ ��� 700 ~ 1000 ���� 
		if (sero > garo && C_area > 600)
		{

			for (int j = 1; j < 4; j++)
			{
				if (max_x < rect_points[j].x)
					max_x = rect_points[j].x;

				if (rect_points[j].x < min_x)
					min_x = rect_points[j].x;

				if (max_y < rect_points[j].y)
					max_y = rect_points[j].y;

				if (rect_points[j].y < min_y)
					min_y = rect_points[j].y;
			}

			candidate[index].max_x = max_x;
			candidate[index].min_x = min_x;
			candidate[index].max_y = max_y;
			candidate[index].min_y = min_y;
			index++;

			// ���� ����X�� ���·� �簢�� �׸��� �κ�
			//for (int j = 0; j < 4; j++)
			//{
			//	line(img, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 4, 8);
			//}
		}

	}

	for (int i = 0; i < 20; i++)
	{
		if (candidate[i].max_x != -1)
		{
			cout << "max_x : " << candidate[i].max_x << endl;
			cout << "min_x : " << candidate[i].min_x << endl;
			cout << "max_y : " << candidate[i].max_y << endl;
			cout << "min_y : " << candidate[i].min_y << endl;
			rectangle(img, Point(candidate[i].min_x - 10, candidate[i].min_y -10), Point(candidate[i].max_x + 10, candidate[i].max_y + 10), Scalar(0, 255, 0), 4);
		}
	}


	show(img);
}


void check_money(Mat img, int this_img, String file_name)
{

	vector<Mat> roi_vector;
	Mat copy = img.clone();
	Mat copy_roi = img.clone();
	Mat last_result;
	Mat gray, roi, t_roi;
	//show(img);
	cvtColor(img, gray, COLOR_BGR2GRAY);

	uint8_t uarr[121];

	for (int i = 0; i < 121; i++)
	{
		if (i > 54 && i < 66)
			uarr[i] = 1;
		else
			uarr[i] = 0;
	}
	Size sz(11, 11);

	Mat linek(sz, CV_8U, uarr);
	/*
	linek �� ����
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	*/

	Mat x, blur, canny, result;


	morphologyEx(gray, x, MORPH_OPEN, linek);
	gray -= x;
	GaussianBlur(gray, blur, Size(5, 5), 0);
	Canny(blur, canny, 100, 120);

	//show(canny);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(1, 3));
	morphologyEx(canny, result, MORPH_ERODE, kernel);

	//show(result);


	kernel = getStructuringElement(MORPH_RECT, Size(30, 1));
	morphologyEx(result, result, MORPH_DILATE, kernel);

	//show(result);

	//������ ã�� minAeraRect����
	vector< vector<Point> > contour, contour_roi;

	findContours(result, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	vector<Rect> boundRect(contour.size());
	vector<RotatedRect> minRect(contour.size());

	// minAreaRect ����
	for (int i = 0; i < contour.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contour[i]));
	}

	// minAreaRect �׸���
	for (int i = 0; i < contour.size(); i++)
	{
		Point2f rect_points[4];
		minRect[i].points(rect_points);

		int max_x = rect_points[0].x;
		int min_x = rect_points[0].x;
		int max_y = rect_points[0].y;
		int min_y = rect_points[0].y;

		for (int j = 1; j < 4; j++)
		{
			if (max_x < rect_points[j].x)
				max_x = rect_points[j].x;

			if (rect_points[j].x < min_x)
				min_x = rect_points[j].x;

			if (max_y < rect_points[j].y)
				max_y = rect_points[j].y;

			if (rect_points[j].y < min_y)
				min_y = rect_points[j].y;
		}


		int garo = max_x - min_x;
		int sero = max_y - min_y;
		int area = (max_x - min_x) * (max_y - min_y);
		int ratio = 0;
		if (garo == 0 || sero == 0)
			ratio = 0;
		else
			ratio = garo / sero;

		if (sero > 7 && sero < 70 && garo < 200 && garo > 70 && ratio > 1 && ratio < 8)
		{
			if (min_x - 15 > 0 && min_y - 15 > 0 && max_x + 15 < 640 && max_y + 15 < 360)
			{
				Rect rect(Point2i(min_x - 15, min_y - 15), Point2i(max_x + 15, max_y + 15));
				roi = copy_roi(rect);
				if (roi.empty())
					throw - 1;
				//show(roi);
				last_roi = roi.clone();
				Mat c_roi = roi.clone();
				Mat gray, result, last, blur;
				//bilateralFilter(c_roi, blur, 40, 150, 200);
				GaussianBlur(c_roi, blur, Size(3, 3), 0);


				//cvtColor(blur, gray, CV_BGR2GRAY);
				Canny(blur, canny, 20, 40);

				//show(canny);

				Mat k = getStructuringElement(MORPH_RECT, Size(1, 5));
				morphologyEx(canny, result, MORPH_ERODE, k);
				//show(result);

				Mat kk = getStructuringElement(MORPH_RECT, Size(3, 1));

				morphologyEx(result, result, MORPH_DILATE, kk);
				//show(result);

				Mat kkk = getStructuringElement(MORPH_RECT, Size(1, 7));
				morphologyEx(result, result, MORPH_CLOSE, kkk);
				//show(result);

				Mat kkkk = getStructuringElement(MORPH_RECT, Size(11, 1));
				morphologyEx(result, result, MORPH_DILATE, kkkk);


				//show(result);

				findContours(result, contour_roi, RETR_EXTERNAL, CHAIN_APPROX_NONE);


				vector<Rect> boundRect(contour_roi.size());
				vector<RotatedRect> minRect(contour_roi.size());
				// minAreaRect ����
				for (int k = 0; k < contour_roi.size(); k++)
				{
					minRect[k] = minAreaRect(Mat(contour_roi[k]));
				}


				for (int i = 0; i < contour_roi.size(); i++)
				{
					Point2f rect_points[4];
					minRect[i].points(rect_points);
					////////////////


					int left_idx1 = 0;
					int left_idx2 = 0;
					int right_idx1 = 0;
					int right_idx2 = 0;

					for (int j = 0; j < 4; j++)
					{

						if (rect_points[left_idx2].x >= rect_points[j].x)
						{
							if (rect_points[left_idx1].x >= rect_points[j].x)
							{
								left_idx2 = left_idx1;
								left_idx1 = j;
								continue;
							}

							left_idx2 = j;

						}

						if (rect_points[right_idx2].x <= rect_points[j].x)
						{
							if (rect_points[right_idx1].x <= rect_points[j].x)
							{
								right_idx2 = right_idx1;
								right_idx1 = j;
								continue;
							}

							right_idx2 = j;
						}

					}


					int leftTop = rect_points[left_idx2].y > rect_points[left_idx1].y ? left_idx1 : left_idx2;
					int leftBottom = left_idx1 == leftTop ? left_idx2 : left_idx1;

					int rightTop = rect_points[right_idx2].y > rect_points[right_idx1].y ? right_idx1 : right_idx2;
					int rightBottom = right_idx1 == rightTop ? right_idx2 : right_idx1;

					float width, height;

					width = sqrtf(powf(rect_points[rightTop].x - rect_points[leftTop].x, 2) + powf(abs(rect_points[rightTop].y - rect_points[leftTop].y), 2));
					height = sqrtf(powf(abs(rect_points[leftTop].x - rect_points[leftBottom].x), 2) + powf(rect_points[leftBottom].y - rect_points[leftTop].y, 2));


					float ratio = 0;
					if (width == 0 || height == 0)
						ratio = 0;
					else
						ratio = (float)width / (float)height;

					if (contourArea(contour_roi[i]) >= 500 && ratio > 2)
					{
						//cout << "index:" << i << "  AREA:" << contourArea(contour_roi[i]) << "  length_true:" <<  arcLength(contour_roi[i], true) << "  length_false:" << arcLength(contour_roi[i], false) << "  ratio:" << ratio << endl;
						//show(roi);

						if (last_roi.empty())
							continue;
						roi_vector.push_back(roi.clone());


					}
				}
			}
		}
	}

	// v_roi �� ���η� ���� �뵵, result�� v_roi �ٽ� ���̴� �뵵
	Mat v_roi_0, v_roi_1, v_roi_2, v_roi_3;
	Mat result0, result1, result2;

	// roi������ 1���϶�
	if (roi_vector.size() == 1)
	{
		Mat save_roi = roi_vector[0];
		resize(save_roi, save_roi, Size(150, 50));
		Mat black_line(20, 150, CV_8UC3, Scalar(0, 0, 0));
		vconcat(save_roi, black_line, v_roi_0); // roi�� ����̹��� �հ� ����
		char roi_name[100];
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d��°�̹���.jpg", this_img);
		imwrite(roi_name, v_roi_0);
	}
	else if ((roi_vector.size() == 2))
	{
		for (int k = 0; k < roi_vector.size(); k++)
		{
			Mat save_roi = roi_vector[k];
			resize(save_roi, save_roi, Size(150, 50));
			Mat black_line(20, 150, CV_8UC3, Scalar(0, 0, 0));
			if (k == 0) vconcat(save_roi, black_line, v_roi_0); // ù roi�� ����̹��� �հ� ����
			else if (k == 1) vconcat(save_roi, black_line, v_roi_1); // �� ° roi�� ����̹��� �հ� ����
		}
		vconcat(v_roi_0, v_roi_1, result0);
		char roi_name[100];
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d��°�̹���.jpg", this_img);
		imwrite(roi_name, result0);
	}
	else if ((roi_vector.size() == 3))
	{
		for (int k = 0; k < roi_vector.size(); k++)
		{
			Mat save_roi = roi_vector[k];
			resize(save_roi, save_roi, Size(150, 50));
			//show(save_roi);
			Mat black_line(20, 150, CV_8UC3, Scalar(0, 0, 0));
			if (k == 0) vconcat(save_roi, black_line, v_roi_0);
			else if (k == 1) vconcat(save_roi, black_line, v_roi_1);
			else if (k == 2) vconcat(save_roi, black_line, v_roi_2);
		}
		vconcat(v_roi_0, v_roi_1, result0);
		vconcat(result0, v_roi_2, result1);
		char roi_name[100];
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d��°�̹���.jpg", this_img);
		imwrite(roi_name, result1);
	}
	else if ((roi_vector.size() == 4))
	{
		for (int k = 0; k < roi_vector.size(); k++)
		{
			Mat save_roi = roi_vector[k];
			resize(save_roi, save_roi, Size(150, 50));
			//show(save_roi);
			Mat black_line(20, 150, CV_8UC3, Scalar(0, 0, 0));
			if (k == 0) vconcat(save_roi, black_line, v_roi_0);
			else if (k == 1) vconcat(save_roi, black_line, v_roi_1);
			else if (k == 2) vconcat(save_roi, black_line, v_roi_2);
			else if (k == 3) vconcat(save_roi, black_line, v_roi_3);
		}
		vconcat(v_roi_0, v_roi_1, result0);
		vconcat(result0, v_roi_2, result1);
		vconcat(result1, v_roi_3, result2);
		char roi_name[100];
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d��°�̹���.jpg", this_img);
		imwrite(roi_name, result2);
	}
	else
	{
		cout << file_name <<"�� �ĺ��� ���ų� �ĺ��� 5�� �̻��̶� �ٽ� ���� ��.." << endl;
	}

}


int main()
{

		// �̹��� ������ ���� ���
		sprintf_s(dir_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test");
		//sprintf_s(dir_name, "C:/Users/kwon/Desktop/cv_3.4.1/test_bill/");
		glob(dir_name, filenames);


		// ��ο� �ִ� ��� �̹����� ���� ����
		for (j = 0; j < filenames.size(); j++)
		{
			//cout << "���� ���� ���� �̹��� " << filenames[j] << endl;

			Mat img = imread(filenames[j]);
			

			// 1280 X 720 �̹����� ����
			if(img.rows != 1280 && img.cols != 720)
				resize(img, img, Size(1280, 720));

			resize(img, img, Size(640, 360));


			/*
			// 850 X 450 ���� CROP�ϴ� �κ�!!
			Mat crop_img(img, Rect(215, 135, 850, 450));

			// �ڸ� �̹����� 640, 360���� �� ����
			resize(crop_img, crop_img, Size(640, 360));

			// �ڸ� �̹��� hsv�� ����� 
			Mat hsv = make_hsv_img(crop_img);

			// �̹��� �ȿ� ���� �ִ��� Ȯ��
			int r = find_hand(hsv, crop_img);
			if (r == 0)
			{
				cout << "frame �ȿ� ���� �����ϴ�." << endl;
				//  find_hand()���� �� ��ã�Ƽ� ����
				return -1;
			}
			
			// �� ������ ���� Ȯ��
			check_money(crop_img, j, filenames[j]);

			*/

			// 5000���� 50000���� �����ϱ� ���ؼ� ���η� ���� �κ� ã�� ����
			vertical_number(img);


			if (j == filenames.size() - 1)
			{
				cout <<"��� �̹��� �Ϸ�" << endl;
				continue;
			}
		}

	

	return 0;
}