rd /s /q "C:\Users\Zan\Desktop\Deployment"
md "C:\Users\Zan\Desktop\Deployment"
robocopy "C:\Users\Zan\Desktop\robocopyUI\x64\Release" "C:\Users\Zan\Desktop\Deployment" ClipboardApp.exe 
robocopy "C:\Users\Zan\Desktop\robocopyUI\x64\Release" "C:\Users\Zan\Desktop\Deployment" CopyApp.exe 
robocopy "C:\Users\Zan\Desktop\robocopyUI\x64\Release" "C:\Users\Zan\Desktop\Deployment" PasteApp.exe
robocopy "C:\Users\Zan\Desktop\robocopyUI" "C:\Users\Zan\Desktop\Deployment" InstallationScript.nsi
