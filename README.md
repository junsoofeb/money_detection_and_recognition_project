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

## 3. 동작 과정

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

ㄹ) 지폐 속 숫자 추출 프로그램

1) grayscale이미지에서 가로 1줄짜리 kernel로 opening연산을 한 이미지를 빼서 길쭉한 세로 픽셀 제거
2) 침식, 팽창 연산을 통해 돈의 숫자 부분의 후보 찾고, findcontour() 적용
3) 데이터에 맞는 가로, 세로, 종횡비 최적화 후 roi 추출 (optimization.txt 참고)
4) 가로, 세로 픽셀을 바꿔가며 침식,팽창연산 후 컨투어 넓이와 종횡비로 최종 후보 추출
5) vconcat을 이용해 모든 후보를 세로로 합쳐서 저장

## 예시
### money recognition
(원본)
<img width="960" alt="5000f_0" src="https://user-images.githubusercontent.com/46870741/66060776-2ff68780-e579-11e9-8241-fe197d03e94d.png">
(결과)
<img width="960" alt="5000f_1" src="https://user-images.githubusercontent.com/46870741/66060822-413f9400-e579-11e9-9ba2-a034f8edecac.png">
### money detection
(지폐 속 숫자 추출)
<img width="367" alt="money" src="https://user-images.githubusercontent.com/46870741/66718470-4de8a580-ee1f-11e9-9e59-ed4501b5d2ce.png">


