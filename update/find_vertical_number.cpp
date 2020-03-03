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


	// 10 X 10 원형 커널로 작은 크기의 노이즈 제거
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
	morphologyEx(blur, hsv, MORPH_CLOSE, kernel);

	//show(hsv);

	return hsv;
}


int find_hand(Mat hsv_img, Mat origin)
{

	Mat temp = hsv_img.clone();
	Mat ori = origin.clone();

	// 손이 자른 이미지안에 있는지 체크 
	int hand_check = 0;

	//컨투어 찾고 minAeraRect생성
	vector< vector<Point> > contour, contour_roi;

	findContours(hsv_img, contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);
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
		int area = contourArea(contour[i]);
		//cout << area << endl;
		// 8000은 test 이미지 10장으로 정함 
		if (area > 8000)
		{
			// draw결과 보려면 주석 제거 
			Point2f rect_points[4];
			minRect[i].points(rect_points);
			drawContours(ori, contour, i, (0, 0, 255), 2, 8);
			hand_check = 1;
		}
	}

	//show(ori);


	// 자른 이미지 안에 손 있으면
	if (hand_check == 1)
		return 1;

	// 없으면,
	else
		return 0;
}


void vertical_number(Mat img)
{

	// 세로 이미지 저장용
	vector<Mat> V_roi_vector;

	Mat copy = img.clone();
	Mat gray, canny, blur, result;
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


	//컨투어 찾고 minAeraRect생성
	vector< vector<Point> > contour, contour_roi;

	findContours(result, contour, RETR_LIST, CHAIN_APPROX_NONE);

	vector<Rect> boundRect(contour.size());
	vector<RotatedRect> minRect(contour.size());

	// minAreaRect 생성
	for (int i = 0; i < contour.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contour[i]));
	}

	// 후보가 몇개인지 카운트
	int count = 0;

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

		int C_area = contourArea(contour[i]);

		// 가로보다 세로가 더 길고, 크기가 기준 이상만 거르기
		// 테스트 결과 700 ~ 1000 사이 
		if (sero > garo && C_area > 600)
			count++;
	}



	//////////////////////////////////
	cout << "후보의 개수 >> " << count << endl;
	if (count > 20)
	{
		cout << "후보가 20개가 넘습니다." << endl;
		return;
	}
	min_max_points candidate[20];
	int index = 0;
	//////////////////////////////////


	// 최대 최소 x,y 좌표 저장 용도

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

		// 가로보다 세로가 더 길고, 크기가 기준 이상만 거르기
		// 테스트 결과 700 ~ 1000 사이 
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

			// 넓이 보정X인 상태로 사각형 그리는 부분
			//for (int j = 0; j < 4; j++)
			//{
			//	line(img, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 4, 8);
			//}
		}

	}


	// 후보들 추출
	for (int i = 0; i < 20; i++)
	{
		if (candidate[i].max_x != -1)
		{
			cout << "max_x : " << candidate[i].max_x << endl;
			cout << "min_x : " << candidate[i].min_x << endl;
			cout << "max_y : " << candidate[i].max_y << endl;
			cout << "min_y : " << candidate[i].min_y << endl;
			rectangle(img, Point(candidate[i].min_x - 10, candidate[i].min_y -10), Point(candidate[i].max_x + 10, candidate[i].max_y + 10), Scalar(0, 255, 0), 4);
			Mat roi(copy, Rect(candidate[i].min_x - 10, candidate[i].min_y - 10, candidate[i].max_x - candidate[i].min_x + 20, candidate[i].max_y - candidate[i].min_y + 20));
			
			V_roi_vector.push_back(roi.clone());
		}
	}

	// 세로로 적힌 이미지들 출력
	for (int i = 0; i < V_roi_vector.size(); i++)
	{
		show(V_roi_vector[i]);
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
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d번째이미지.jpg", this_img);
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
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d번째이미지.jpg", this_img);
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
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d번째이미지.jpg", this_img);
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
		sprintf_s(roi_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test_roi/%d번째이미지.jpg", this_img);
		imwrite(roi_name, result2);
	}
	else
	{
		cout << file_name <<"는 후보가 없거나 후보가 5개 이상이라 다시 찍어야 함.." << endl;
	}

}


int main()
{

		// 이미지 파일의 절대 경로
		sprintf_s(dir_name, "C:/Users/feb25/Desktop/OpenCV_cpp/test");
		//sprintf_s(dir_name, "C:/Users/kwon/Desktop/cv_3.4.1/test_bill/");
		glob(dir_name, filenames);


		// 경로에 있는 모든 이미지에 대해 수행
		for (j = 0; j < filenames.size(); j++)
		{
			//cout << "현재 진행 중인 이미지 " << filenames[j] << endl;

			Mat img = imread(filenames[j]);
			

			// 1280 X 720 이미지로 가정
			if(img.rows != 1280 && img.cols != 720)
				resize(img, img, Size(1280, 720));

			resize(img, img, Size(640, 360));


			/*
			// 850 X 450 으로 CROP하는 부분!!
			Mat crop_img(img, Rect(215, 135, 850, 450));

			// 자른 이미지를 640, 360으로 재 조절
			resize(crop_img, crop_img, Size(640, 360));

			// 자른 이미지 hsv로 만들기 
			Mat hsv = make_hsv_img(crop_img);

			// 이미지 안에 손이 있는지 확인
			int r = find_hand(hsv, crop_img);
			if (r == 0)
			{
				cout << "frame 안에 지폐가 없습니다." << endl;
				//  find_hand()에서 손 못찾아서 종료
				return -1;
			}
			
			// 손 있으면 지폐 확인
			check_money(crop_img, j, filenames[j]);

			*/

			// 5000인지 50000인지 구별하기 위해서 세로로 적힌 부분 찾는 과정
			vertical_number(img);
			


			if (j == filenames.size() - 1)
			{
				cout <<"모든 이미지 완료" << endl;
				continue;
			}
		}



	return 0;
}