import cv2 as cv
import os
import os.path
import numpy as np
#from PIL import Image
#from pytesseract import *

def show(img):
    cv.imshow("", img)
    cv.waitKey()
    cv.destroyAllWindows()


def check_front(img):
    copy = img.copy()
    copy_roi = img.copy()
    #show(img)   
    gray=cv.cvtColor(img,cv.COLOR_BGR2GRAY)
    linek = np.zeros((11,11),dtype=np.uint8)
    linek[5,...]=1
    #print(linek)
    x = cv.morphologyEx(gray, cv.MORPH_OPEN, linek ,iterations=1)
    gray -= x
    #cv.imshow('gray',gray)
    #cv.waitKey(0)         
    blur = cv.GaussianBlur(gray, (5,5),0)
    canny  = cv.Canny(blur, 100, 150)
    #show(canny)
    
    kernel = np.ones((3, 1), np.uint8)    
    result = cv.morphologyEx(canny, cv.MORPH_ERODE, kernel)
    #show(result)
    
    kernel = np.ones((1, 40), np.uint8)    
    result = cv.morphologyEx(result, cv.MORPH_DILATE, kernel)
    #show(result)
    cnts = cv.findContours(result, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)[0]
    for j in range(len(cnts)):
        cnt = cnts[j]
        area = cv.contourArea(cnt)
        length = cv.arcLength(cnt, True)
        rect = cv.minAreaRect(cnt)
        box = cv.boxPoints(rect)
        box = np.int0(box)        
        #cv.drawContours(copy, [box], -1, (0,0,255),2)
                
        min_x = min(box[:, 0])
        max_x = max(box[:, 0])
        min_y = min(box[:, 1])
        max_y = max(box[:, 1])
        
        garo = max_x - min_x
        sero = max_y - min_y
        area = (max_x - min_x) * (max_y - min_y)
        
        ratio = garo // sero
        # 넓이, 가로 길이, 세로길이, 종횡비

        # 가로 세로 종횡비로 1차 거르기
        if (sero < 65 and sero > 8 ) and (garo > 90 and garo < 300) and ratio >= 2:
            # x좌표가 왼쪽 절반에 있을때만
            if min_x < 320:
                
                try:
                    roi = copy_roi[min_y - 15: max_y + 10, min_x - 10 : max_x + 10]
                    #show(roi)
                    c_roi = roi.copy()
                    blur = cv.GaussianBlur(roi, (9, 9), 0)
                    edge = cv.Canny(blur, 100, 150)
                    #show(edge)
                    r_cnts = cv.findContours(edge, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)[0]
                    # 컨투어 박스의 개수 카운트
                    box_cnt = 0
                    for jj in range(len(r_cnts)):
                        try:
                            r_cnt = r_cnts[jj]
                            rect = cv.minAreaRect(r_cnt)
                            box = cv.boxPoints(rect)
                            box = np.int0(box)   
                            box_cnt += 1     
                            #cv.drawContours(c_roi, [box], -1, (0,0,255),2)
                        except :
                            continue
                    # 컨투어 박스의 개수로 최종 결정.. 이 부분이 최종 roi
                    if box_cnt > 2 and box_cnt < 11:
                        cv.rectangle(copy, (min_x - 15, min_y - 10), (max_x + 10 , max_y + 10), (0,0,255), 2)
                except :
                    continue
    show(copy)

def check_rear(img):
    copy = img.copy()
    copy_roi = img.copy()
    #show(img)   
    gray=cv.cvtColor(img,cv.COLOR_BGR2GRAY)
    linek = np.zeros((11,11),dtype=np.uint8)
    linek[5,...]=1
    #print(linek)
    x = cv.morphologyEx(gray, cv.MORPH_OPEN, linek ,iterations=1)
    gray -= x
    #cv.imshow('gray',gray)
    #cv.waitKey(0)         
    blur = cv.GaussianBlur(gray, (5,5),0)
    canny  = cv.Canny(blur, 100, 150)
    #show(canny)
    
    kernel = np.ones((3, 1), np.uint8)    
    result = cv.morphologyEx(canny, cv.MORPH_ERODE, kernel)
    #show(result)
    
    kernel = np.ones((1, 40), np.uint8)    
    result = cv.morphologyEx(result, cv.MORPH_DILATE, kernel)
    #show(result)
    cnts = cv.findContours(result, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)[0]
    for j in range(len(cnts)):
        cnt = cnts[j]
        area = cv.contourArea(cnt)
        length = cv.arcLength(cnt, True)
        rect = cv.minAreaRect(cnt)
        box = cv.boxPoints(rect)
        box = np.int0(box)        
        #cv.drawContours(copy, [box], -1, (0,0,255),2)
                
        min_x = min(box[:, 0])
        max_x = max(box[:, 0])
        min_y = min(box[:, 1])
        max_y = max(box[:, 1])
        
        garo = max_x - min_x
        sero = max_y - min_y
        area = (max_x - min_x) * (max_y - min_y)
        
        ratio = garo // sero

        if (sero < 40  and sero > 0 ) and (garo > 70 and garo < 200) and ratio >= 2:
            # x좌표가 왼쪽 절반에 있을때만
            if min_x > 320:
                
                try:
                    roi = copy_roi[min_y - 15: max_y + 10, min_x - 10 : max_x + 10]
                    #show(roi)
                    c_roi = roi.copy()
                    blur = cv.GaussianBlur(roi, (9, 9), 0)
                    edge = cv.Canny(blur, 100, 150)
                    #show(edge)
                    r_cnts = cv.findContours(edge, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)[0]
                    # 컨투어 박스의 개수 카운트
                    box_cnt = 0
                    for jj in range(len(r_cnts)):
                        try:
                            r_cnt = r_cnts[jj]
                            rect = cv.minAreaRect(r_cnt)
                            box = cv.boxPoints(rect)
                            box = np.int0(box)   
                            box_cnt += 1     
                            #cv.drawContours(c_roi, [box], -1, (0,0,255),2)
                        except :
                            continue
                    # 컨투어 박스의 개수로 최종 결정.. 이 부분이 최종 roi
                    if box_cnt > 1 and box_cnt < 11:
                    #if 1:
                        cv.rectangle(copy, (min_x - 15, min_y - 10), (max_x + 10 , max_y + 10), (0,0,255), 2)
                    #print(box_cnt)
                except :
                    continue
    show(copy)

def main():
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

        # 빠른 확인을 위해 직접 돈의 유형 지정
        #money = '5000' 
        #direct = 'f'

        img_path = f'./money_img/{money}_{direct}'
        img_list = list()

        for file in os.listdir(img_path):
            img = os.path.join(img_path, file) 
            img_list.append(img)

        for i in range(len(img_list)):
            # 빠른 확인을 위해 직접 이미지 지정 
            # i = 13
            print("%d번째 이미지" % (i + 1))
            img = cv.imread(img_list[i])
            img = cv.resize(img, (640, 360))
            
            # 앞면이라고 가정하고 roi 체크
            check_front(img)
            # 뒷면이라고 가정하고 roi 체크
            check_rear(img)
            
main()
