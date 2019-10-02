# money_recognition_project
with OpenCV

## 1. 개요

카메라로 지폐가 얼마짜리 지폐인지 인식하는 프로그램

## 2. 구현 환경

1) window10 Home
2) Python 3.6
3) OpenCV-python library 4.1.0
4) Numpy 1.16
5) tesseract-3.05.01-vc14_10 및 pytesseract 0.3.0

## 동작 과정

#### ㄱ, ㄴ 은 지폐의 앞면(인물이 있는 면)만 동작, ㄷ은 양 면 모두 동작

ㄱ) 한글 인식 버전

ex) 지폐 속의 오천원, 만원 등을 인식

1) 이미지 resize(), bilateralFilter() , morphologyEx() 등을 통해 지폐가 있는 roi 추출 
2) 추출한 roi에서 contour의 길이, 넓이로 sub_roi추출(한글 부분)
3) sub_roi에 tesseract를 적용하여 지폐 인식

ㄴ) 숫자 인식 버전 v1

ex) 지폐 속의 5000, 10000 등을 인식

1) 이미지에서 모폴로지 연산을 통해 roi 추출
2) 추출한 roi에서 contour의 길이, 넓이로 sub_roi추출(숫자 부분)
3) sub_roi에 tesseract를 적용하여 지폐 인식

ㄷ) 숫자 인식 버전 v2

1) grayscale 이미지에서 모폴로지 연산을 통해 전처리
2) 가로 길이, 세로 길이, 종횡비를 통해 경우의 수 줄이기
3 - 1) 앞면인 경우, x 좌표가 왼쪽 절반에 있는 것을 roi로 설정
3 - 2) 뒷면인 경우, x 좌표가 오른쪽 절반에 있는 것을 roi로 설정
4) roi의 contour 개수가 적당한 것을 최종 roi로 지정

## 예시
원본)
<img width="960" alt="5000f_0" src="https://user-images.githubusercontent.com/46870741/66060776-2ff68780-e579-11e9-8241-fe197d03e94d.png">
추출)
<img width="960" alt="5000f_1" src="https://user-images.githubusercontent.com/46870741/66060822-413f9400-e579-11e9-9ba2-a034f8edecac.png">

