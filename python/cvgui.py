import cv2
cv2.namedWindow("left",cv2.WINDOW_NORMAL)
cv2.namedWindow("right",cv2.WINDOW_NORMAL)
cv2.namedWindow("control".cv2.WINDOW_NORMAL)
while True:
  capL = cv2.VideoCapture()
  capR = cv2.VideoCapture()
  capL.open('data/2out1.avi')
  capR.open('data/2out2.avi')
  while cv2.waitKey(1000/15)==-1:
    retval,imgL = capL.read()
    if not retval: break
    retval,imgR = capR.read()
    if not retval: break
    cv2.imshow('left',imgL)
    cv2.imshow('right',imgR)
  
  
