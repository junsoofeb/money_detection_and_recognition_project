#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int index = 0;
int money = 0;
char direct = 0;
int j;
int cnt = 0;
// 사진 저장용
char dir_name[100];
vector<String> filenames;
Mat last_roi;
void show(Mat img)
{
	imshow("", img);
	waitKey(0);
	destroyAllWindows();
}

void check_money(Mat img, int this_img)
{

	vector<Mat> roi_vector;
	Mat copy = img.clone();
	Mat copy_roi = img.clone();
	Mat last_result;
	Mat gray, roi, t_roi;
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
	linek 의 형태
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
	
	//컨투어 찾고 minAeraRect생성
	vector< vector<Point> > contour, contour_roi;

	findContours(result, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	vector<Rect> boundRect(contour.size());
	vector<RotatedRect> minRect(contour.size());

	// minAreaRect 생성
	for (int i = 0; i < contour.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contour[i]));
	}

	// minAreaRect 그리기
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

		if (sero > 7 && sero < 70 && garo < 200 && garo > 70 && ratio > 1 && ratio < 8 )
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


			///	show(result);

				findContours(result, contour_roi, RETR_EXTERNAL, CHAIN_APPROX_NONE);


				vector<Rect> boundRect(contour_roi.size());
				vector<RotatedRect> minRect(contour_roi.size());
				// minAreaRect 생성
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
						ratio = (float)width / (float)height ;

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

	// v_roi 는 세로로 붙일 용도, result는 v_roi 다시 붙이는 용도
	Mat v_roi_0, v_roi_1, v_roi_2, v_roi_3;
	Mat result0, result1, result2;

	// roi개수가 1개일때
	if (roi_vector.size() == 1)
	{
		Mat save_roi = roi_vector[0];
		resize(save_roi, save_roi, Size(150, 50));
		Mat black_line(20, 150, CV_8UC3, Scalar(0, 0, 0));
		vconcat(save_roi, black_line, v_roi_0); // roi와 경계이미지 잇고 저장
		char roi_name[100];
		sprintf_s(roi_name,"C:/Users/feb25/Desktop/save_vconcat/%d%c의_%d번째이미지.jpg", money, direct, this_img); // 1000_f_j번째 이미지의 l
		imwrite(roi_name, v_roi_0);
	}
	else if ((roi_vector.size() == 2))
	{
		for (int k = 0; k < roi_vector.size(); k++)
		{
			Mat save_roi = roi_vector[k];
			resize(save_roi, save_roi, Size(150, 50));
			Mat black_line(20, 150, CV_8UC3, Scalar(0, 0, 0));
			if (k == 0) vconcat(save_roi, black_line, v_roi_0); // 첫 roi와 경계이미지 잇고 저장
			else if (k == 1) vconcat(save_roi, black_line, v_roi_1); // 둘 째 roi와 경계이미지 잇고 저장
		}
		vconcat(v_roi_0, v_roi_1, result0);
		char roi_name[100];
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/save_vconcat/%d%c의_%d번째이미지.jpg", money, direct, this_img); // 1000_f_j번째 이미지의 l
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
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/save_vconcat/%d%c의_%d번째이미지.jpg", money, direct, this_img); // 1000_f_j번째 이미지의 l
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
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/save_vconcat/%d%c의_%d번째이미지.jpg", money, direct, this_img); // 1000_f_j번째 이미지의 l
		imwrite(roi_name, result2);
	}
	else
	{
		cout << " 후보가 없거나 후보가 5개 이상이라 다시 찍어야 함.." << endl;
	}




	/*
	for (int l = 0; l < roi_vector.size(); l++)
	{
		//cout << "roi 저장!" << endl;
		char roi_name[100];
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/save/%d%c의_%d번째이미지_%d.jpg", money, direct, this_img, l); // 1000_f_j번째 이미지의 l
		Mat save = roi_vector[l];
		if (save.empty())
			continue;
		imwrite(roi_name, save);
	}
	*/
	//cout << "save finish!" << endl;
	//show(last_roi);
}


int main()
{

	for (int i = 0; i < 8; i++)
	{
		if (i == 0)
		{
			money = 1000;
			direct = 'f';
		}
		else if (i == 1)
		{
			money = 1000;
			direct = 'b';
		}
		else if (i == 2)
		{
			money = 5000;
			direct = 'f';
		}
		else if (i == 3)
		{
			money = 5000;
			direct = 'b';
		}
		else if (i == 4)
		{
			money = 10000;
			direct = 'f';
		}
		else if (i == 5)
		{
			money = 10000;
			direct = 'b';
		}
		else if (i == 6)
		{
			money = 50000;
			direct = 'f';
		}
		else
		{
			money = 50000;
			direct = 'b';
		}

		index = i;

		// 이미지 파일의 절대 경로
		sprintf_s(dir_name, "C:/Users/feb25/Desktop/bills/%d%c", money, direct);
		glob(dir_name, filenames);

		for (j = 0; j < filenames.size(); j++)
		{
			cout << "현재 진행 중인 이미지 " << filenames[j] << endl;

			Mat img = imread(filenames[j]);
			resize(img, img, Size(640, 360));
			check_money(img, j);
			cnt++;
			//check_rear(img, j);


			if (j == filenames.size() - 1)
			{
				cout << money << '_' << direct << " 완료" << endl;
				continue;
			}
		}

	}

	return 0;
}