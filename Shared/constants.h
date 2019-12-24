#ifndef CONSTANTS_H
#define CONSTANTS_H 


#define MAXPATHLEN 1024
#define MAXITEMCOUNT 100000
#define MAXCONTENTSSIZE (MAXPATHLEN + 1) * (MAXITEMCOUNT + 1)
#define MAXRUNNINGTIME INFINITE // in miliseconds 
#define MUTEXNAME_SINGLECLIPBOARDAPP L"ClipboardAppMutex_48031724+Jovan-Zan@users.noreply.github.com"
#define MUTEXNAME_MMFMUTEX L"MMFMutex_48031724+Jovan-Zan@users.noreply.github.com"
#define EVENTNAME_AQUIREMMFHANDLE L"AquireMMFHandle_48031724+Jovan-Zan@users.noreply.github.com"
#define mmfName L"robocopyUI_mmf" // Memory mapped file name.

#endif // !CONSTANTS_H
