import cv2 as cv
import os
import os.path
import numpy as np
from PIL import Image
from pytesseract import *

def show(img):
    cv.imshow("", img)
    cv.waitKey()
    cv.destroyAllWindows()

# img_path는 환경에 맞게 수정 필요!
# 본인의 이미지 경로를 지정!
img_path = './money_img/5000'  
img_list = list()

for file in os.listdir(img_path):
    img = os.path.join(img_path, file) 
    img_list.append(img)


for i in range(len(img_list)):
    OCR_SIGN = 0
    
    print("%d번째 이미지" % (i + 1))
    img = cv.imread(img_list[i])
    img = cv.resize(img, (640, 480))
    
    show(img)
    
    copy = img.copy()
    gray = cv.cvtColor(img ,cv.COLOR_BGR2GRAY)
    
    # 최적화 과정
    #for j in range(0,256,8):
    #    print("current j :", j)
    #    blur = cv.bilateralFilter(gray, 8, j,j)
    #    thre = cv.threshold(blur, 0, 255, cv.THRESH_BINARY | cv.THRESH_OTSU)[1]
    #    show(thre)
    
    '''
    bilateralFilter(src, d, sigmaColor, sigmaSpace)
    
    d – 필터링 중에 사용되는 각 픽셀 주변의 거리. 음수로 주면 sigmaSpace에서 계산.
    sigmaColor – 색 공간에서 시그마를 필터링. 
                매개 변수의 값이 클수록 픽셀 인접 영역 (sigmaSpace 참조)에서 더 멀리있는 색상이 함께 혼합되어 
                더 넓은 영역의 준 균일 색상.
    sigmaSpace – 좌표 공간에서 시그마를 필터링. 
                매개 변수의 값이 클수록 색상이 충분히 가까이있는 한 픽셀이 서로 영향을 줄 수 있다. (sigmaColor 참조). 
                d > 0 이면 sigmaSpace에 관계없이 인접 크기를 지정. 
                d < 0 이면 d는 sigmaSpace에 비례.
    '''

    blur = cv.bilateralFilter(gray, 50, 200, 40)
    #show(blur)
    #edge = cv.Canny(blur, 0, 40)
    
    edge = cv.Canny(blur, 0, 50)
    show(edge)
    kernel = np.ones((1, 300), np.uint8)
    result = cv.morphologyEx(edge, cv.MORPH_DILATE, kernel)
    show(result)

    cnts = cv.findContours(result, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)[0]
    
    
    # 가장 큰 영역만 roi로 추출
    biggest = 0    
    biggest_index = None
    for j in range(len(cnts)):
        cnt = cnts[j]
        area = cv.contourArea(cnt)
        if biggest < area:
            biggest = area
            biggest_index = j
    
    # 가장 큰 영역에 빨갛게 minAreaRect() 만들기    
    cnt = cnts[biggest_index]
    rect = cv.minAreaRect(cnt)
    box = cv.boxPoints(rect)
    box = np.int0(box)
    
    cv.drawContours(copy, [box], -1, (0,0,255), 2)
    show(copy)
    
    min_x = min(box[:, 0])
    max_x = max(box[:, 0])
    min_y = min(box[:, 1])
    max_y = max(box[:, 1])
    
    # roi 추출
    roi = img[min_y : max_y , min_x : max_x]
    copy_roi = roi.copy()
    #show(roi)

    gray = cv.cvtColor(roi ,cv.COLOR_BGR2GRAY)
    blur = cv.bilateralFilter(gray, 8, 40, 40)
    #blur = cv.bilateralFilter(gray, 45, 255, 40)
    edge = cv.Canny(blur, 0, 40)
    #show(edge)
    thre = cv.threshold(blur, 0, 255, cv.THRESH_BINARY_INV | cv.THRESH_OTSU)[1]
    
    rect_kernel = np.ones((1, 17), np.uint8)
    dilation = cv.dilate(thre, rect_kernel, iterations = 1)
    #show(dilation)
    
    kernel = np.ones((2, 10), np.uint8)
    erosion = cv.erode(dilation, kernel, iterations = 1)
    #show(erosion)

    cnts = cv.findContours(erosion, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)[0]
    for j in range(len(cnts)):
        cnt = cnts[j]
        area = cv.contourArea(cnt)
        length = cv.arcLength(cnt, True)
        if area < 1500 and area > 300 and length < 1000 and length > 100:
            #print("index >>", j,"  AREA >>", area, "  length() >>", length)
            rect = cv.minAreaRect(cnt)
            box = cv.boxPoints(rect)
            box = np.int0(box)
            cv.drawContours(roi, [box], -1, (0,0,255),1)
            min_x = min(box[:, 0])
            max_x = max(box[:, 0])
            min_y = min(box[:, 1])
            max_y = max(box[:, 1])
    
            # roi 안에서 한글 부분만 추출 
            sub_roi = copy_roi[min_y - 5: max_y + 5, min_x - 5 : max_x + 5]

            try:     
                sub_roi = cv.cvtColor(sub_roi, cv.COLOR_BGR2GRAY)
                filter= cv.bilateralFilter(sub_roi, 5, 100, 75)
                #filter= cv.bilateralFilter(sub_roi, 5, 100, 75)
                filter = cv.threshold(filter, 0, 255, cv.THRESH_BINARY | cv.THRESH_OTSU)[1]
                
                #cv.addWeighted(sub_roi, 2, filter, -1.2, 0 , filter)
                text = image_to_string(filter, lang = 'kor', config='--psm 10 -c preserve_interword_spaces=1')

                if '천' in text:
                    if '오' in text or '호' in text:
                        OCR_SIGN = 1
                        print("오 천 원")
                        show(filter)
                        break
                    else:
                        OCR_SIGN = 1
                        print("천 원")
                        show(filter)
                        break
                                            
                elif '만' in text or '반' in text or '안' in text :
                    if '오' in text or '호' in text:
                        OCR_SIGN = 1
                        print("오 만 원")
                        show(filter)
                        break
                    else:
                        OCR_SIGN = 1
                        print("만 원")
                        show(filter)
                        break
                        
                elif '오' in text or '호' in text:
                    if '만' in text :
                        OCR_SIGN = 1
                        print("오 만 원")
                        show(filter)
                        break
                    elif '천' in text:
                        OCR_SIGN = 1
                        print("오 천 원")
                        show(filter)
                        break
            except :
                print('ROI ERROR : this image was ignored')
                continue
    if OCR_SIGN == 0 :
        print("인식 실패")

    show(roi)
