;Starting the program when Win+O is pressed



#o::
;Starting CyberLinkYouCam for obtaining input
Run, C:\Program Files (x86)\CyberLink\YouCam\YouCam.exe
WinActivate, YouCam
WinWaitActive, YouCam
WinHide, YouCam
WinHide, CyberLink

Run C:\Users\CVIP3D\Documents\Visual Studio 2010\Projects\OPENCV_FACETracking1\Release\OPENCV_FACETracking1.exe
WinWait, Rectangle	

;Hide command window
WinHide, C:\Users\CVIP3D\Documents\Visual Studio 2010\Projects\OPENCV_FACETracking1\Release\OPENCV_FACETracking1

;Setting "Rectangle" window to be always on top other windows
Winset, Alwaysontop, ,Rectangle

;Removing "Rectange" window captions
Winset, Style, -0xC400000, Rectangle
return


;Hide "Rectangle" window when Win+h is pressed and the window is active
#h::
;IfWinActive, Rectangle
WinHide, Rectangle
return

;Show "Rectangle" window when Win+s is pressed and the window is hidden
#s::
WinShow, Rectangle
return 

;Close the program if one exist
#c::
;IfWinExist, Rectangle
WinShow, C:\Users\CVIP3D\Documents\Visual Studio 2010\Projects\OPENCV_FACETracking1\Release\OPENCV_FACETracking1
WinClose, C:\Users\CVIP3D\Documents\Visual Studio 2010\Projects\OPENCV_FACETracking1\Release\OPENCV_FACETracking1.exe
WinShow,YouCam
WinClose,YouCam
Reload
return 