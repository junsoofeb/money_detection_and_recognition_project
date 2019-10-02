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
    
    

for i in range(8):
    if i == 0:
        money = '1000'
        direct = 'f'
    elif i == 1:
        money = '1000'
        direct = 'b'
        
    elif i == 2:
        money = '5000'
        direct = 'f'

    elif i == 3:
        money = '5000'
        direct = 'b'

    elif i == 4:
        money = '10000'
        direct = 'f'
        
    elif i == 5:
        money = '10000'
        direct = 'b'
    elif i == 6:
        money = '50000'
        direct = 'f'
    else:
        money = '50000'
        direct = 'b'
    
    #money = 
    #direct = 
    img_path = f'./money_img/{money}_{direct}'
    img_list = list()

    for file in os.listdir(img_path):
        img = os.path.join(img_path, file) 
        img_list.append(img)


    for i in range(len(img_list)):
        print("%d번째 이미지" % (i + 1))
        img = cv.imread(img_list[i])
        img = cv.resize(img, (640, 480))

        copy = img.copy()
        copy_roi = img.copy()

        show(img)

        copy = img.copy()
        gray = cv.cvtColor(img ,cv.COLOR_BGR2GRAY)
        #blur = cv.bilateralFilter(gray, 50, 200, 40)
        blur = cv.GaussianBlur(gray, (5,5),0)
        #show(blur)
        # edge = cv.Canny(blur, 0, 40)
        edge = cv.Canny(blur, 100, 200)
        show(edge)

        # 세로 5 가로 1 커널로 침식연산
        kernel = np.ones((5, 1), np.uint8)    
        result = cv.morphologyEx(edge, cv.MORPH_ERODE, kernel)
        show(result)

        # 세로 3 가로 20 커널로 팽창연산
        kernel = np.ones((3, 20), np.uint8)
        result = cv.morphologyEx(result, cv.MORPH_DILATE, kernel)
        show(result)

        #kernel = np.ones((5, 1), np.uint8)
        #result = cv.morphologyEx(result, cv.MORPH_ERODE, kernel)
        #show(result)

        # 세로 5 가로 2 커널로 팽창연산
        kernel = np.ones((5, 2), np.uint8)
        result = cv.morphologyEx(result, cv.MORPH_DILATE, kernel)
        show(result)



        cnts = cv.findContours(result, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)[0]

        for j in range(len(cnts)):
            cnt = cnts[j]
            area = cv.contourArea(cnt)
            length = cv.arcLength(cnt, True)
            if area > 1000  and length > 150:
                #print("index >>", j,"  AREA >>", area, "  length() >>", length)
                rect = cv.minAreaRect(cnt)
                box = cv.boxPoints(rect)
                box = np.int0(box)
                cv.drawContours(copy, [box], -1, (0,0,255),2)
                min_x = min(box[:, 0])
                max_x = max(box[:, 0])
                min_y = min(box[:, 1])
                max_y = max(box[:, 1])
                
                sero = max_y - min_y 
                # roi 안에서 한글 부분만 추출 
                
                try:
                    if sero < 40 and sero > 20:
                        sub_roi = copy_roi[min_y - 5: max_y + 5, min_x - 10 : max_x + 10]
                        cv.imwrite("./roi_money/roi_{i}_{j}_{k}.jpg".format(i = money, j = j, k = i + 1 ),sub_roi)
                        show(sub_roi)
                        
                        target = cv.imread("./roi_money/roi_{i}_{j}_{k}.jpg".format(i = money, j = j, k = i + 1 ))
                        target = cv.bilateralFilter(target, 5, 100, 75)
                        target = cv.cvtColor(target, cv.COLOR_BGR2GRAY)
                        target = cv.threshold(target, 0, 255, cv.THRESH_BINARY | cv.THRESH_OTSU)[1]
                
                        #show(target)
                        text = image_to_string(target, config='--psm 10 -c preserve_interword_spaces=1')
                        #print(text)
                        if '1000' in text or '100O' in text:
                            if '10000' in text or '1000O' in text:
                                print('만 원')
                                continue 
                            print('천 원')
                            continue 
                        
                        elif '5000' in text:
                            if '50000' in text or '5OOOO' in text:
                                print('오 만 원')
                                continue 
                            print('오 천 원')
                            continue 
                        
                    elif sero < 20:
                        sub_roi = copy_roi[min_y - 20: max_y + 20, min_x - 25 : max_x + 40]
                        cv.imwrite("./roi_money/roi_{i}_{j}_{k}.jpg".format(i = money, j = j, k = i + 1),sub_roi)
                        show(sub_roi)
                        
                        target = cv.imread("./roi_money/roi_{i}_{j}_{k}.jpg".format(i = money, j = j, k = i + 1 ))
                        target = cv.bilateralFilter(target, 5, 100, 75)
                        target = cv.cvtColor(target, cv.COLOR_BGR2GRAY)
                        target = cv.threshold(target, 0, 255, cv.THRESH_BINARY | cv.THRESH_OTSU)[1]
                
                        #show(target)
                        text = image_to_string(target, config='--psm 10 -c preserve_interword_spaces=1')
                        if '1000' in text or '100O' in text:
                            if '10000' in text or '1000O' in text:
                                print('만 원')
                                continue 
                            print('천 원')
                            continue 
                        
                        elif '5000' in text:
                            if '50000' in text or '5OOOO' in text:
                                print('오 만 원')
                                continue 
                            print('오 천 원')
                            continue 
                        

                except :
                    print("roi error!")
                    continue
        show(copy)
