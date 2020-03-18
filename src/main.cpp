#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <dirent.h>
#include <bits/stdc++.h>
#include <DxLib/DxLib.h>
#include <sys/stat.h>
#include "button.h"
#include "checkbox.h"

#define RECORD_MAX 512

typedef struct{
    char fileName[FILENAME_MAX]; //クリスタファイル名
    int sectime; //このファイルを触った時間
}CLIPSTUDIO_TIME;

struct SETTING{
    bool frontWindow;
    bool activeFlag;
}settingFlags = {true, true};

void save_time(CLIPSTUDIO_TIME *ct, char *name, int time);
int get_starttime(CLIPSTUDIO_TIME *ct, char *name);
char *openClipFile(char *filepath, char init);
void nameCheck(CLIPSTUDIO_TIME *ct, char *folderpath);
char *get_extension(char *filename);

time_t programStartTime = time(NULL);

int font_UI;

int initialize_DxLib(){

    SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	SetGraphMode(256, 256, 32);
	SetWindowText("CLIP Timer");
	if(DxLib_Init() == -1){
        return -1;
    }
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);
    SetWindowSize(256, 128);
	SetWindowIconID(1);
	SetMouseDispFlag(TRUE);
    SetAlwaysRunFlag(TRUE);
    SetBackgroundColor(255, 255, 255);

    return 0;
}

void remove_enter(char *str){
    for(int i = 0; str[i] != '\0'; i++){
        if(str[i] == '\n'){
            str[i] = '\0';
        }
    }
}

int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow)
{

    if(initialize_DxLib()){
        fprintf(stderr, "Some error has occurred in DxLib_Init().\n");
		return -1;
    }

    HWND mainWindow = GetMainWindowHandle();
    SetWindowPos(mainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    font_UI = CreateFontToHandle((const TCHAR *)"Yu Gothic UI", 16, 1, DX_FONTTYPE_NORMAL);

    //char passbuf[FILENAME_MAX] = "\"C:/Users/shake/MyTools/BFI.exe\"";
    char passbuf[FILENAME_MAX] = "\"C:/Program Files/CELSYS/CLIP STUDIO 1.5/CLIP STUDIO/CLIPStudio.exe\"";
    char tmp_passbuf[FILENAME_MAX];
    char clipstudioDir[FILENAME_MAX] = "C:/Users/shake/Documents/C/cli_stu_timer";

    FILE *setfp = fopen("setting.txt", "r");
    if(setfp != NULL){
        fgets(tmp_passbuf, FILENAME_MAX, setfp);
        remove_enter(tmp_passbuf);
        snprintf(passbuf, FILENAME_MAX, "\"%s\"", tmp_passbuf);
        fgets(clipstudioDir, FILENAME_MAX, setfp);
        fclose(setfp);
    }

    FILE *flagsetfp = fopen("flags.bin", "rb");
    if(flagsetfp != NULL){
        fread(&settingFlags, sizeof(struct SETTING), 1, flagsetfp);
        fclose(flagsetfp);
    }
    
    time_t start;
    time_t beforeLossStart;
    int lossTime = 0;
    char Lossing = 0;

    CLIPSTUDIO_TIME ct[RECORD_MAX];
    memset(ct, 0, sizeof(CLIPSTUDIO_TIME) * RECORD_MAX);

    start = time(NULL);

    FILE *ctfp = fopen("timedata.dat", "rb");
    if(ctfp != NULL){
        fread(ct, sizeof(CLIPSTUDIO_TIME), RECORD_MAX, ctfp);
        fclose(ctfp);
    }

    DWORD wait_stat;
    char exit_flag = 0;

    char before_filename[FILENAME_MAX] = {'\0'};
    char *clipfilename = NULL;

    int tmptime = 0;

    Button settingButton;
    settingButton.init(200, 100, 50, 20, "設定");
    Button namecheckButton;
    namecheckButton.init(10, 228, 150, 20, "不要なタイムを削除");
    CheckBox frontcheck;
    frontcheck.init(10, 138, 15, 15, "", 255, 255, 255);
    CheckBox activecheck;
    activecheck.init(10, 158, 15, 15, "", 255, 255, 255);
    char setting = 0;
    frontcheck.setStatus(settingFlags.frontWindow);
    activecheck.setStatus(settingFlags.activeFlag);
    
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    ZeroMemory(&si, sizeof(si));
    si.cb=sizeof(si);

    HWND clipStudioWindow = FindWindow(NULL, TEXT("CLIP STUDIO PAINT"));
    DWORD clipStudioProcessID;
    HANDLE clipStudioProcessHandle;
    if(clipStudioWindow == NULL){
        if(!CreateProcess(NULL, (LPTSTR)passbuf, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)){
            printf("Error : CLIP STUDIO PAINT cannot start.\n");
            DxLib_End();
            exit(-1);
        }
        clipStudioProcessHandle = pi.hProcess;
        clipStudioProcessID = pi.dwProcessId;
    }else{
        printf("clip studio window handle : %d\n", clipStudioWindow);
        GetWindowThreadProcessId(clipStudioWindow, &clipStudioProcessID);
        printf("clip studio PID : %d\n", clipStudioProcessID);
        clipStudioProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, clipStudioProcessID);
    }

    while(!exit_flag && ProcessMessage() == 0){
        ClearDrawScreen();
		SetDrawScreen(DX_SCREEN_BACK);

        //描画関係
        DrawBox(0, 0, 256, 180, GetColor(255, 255, 255), TRUE);
        if(clipfilename != NULL){
            DrawFormatString(10, 0, GetColor(0, 0, 0), (const TCHAR *)"%s", clipfilename);
            if(Lossing == 0)tmptime = get_starttime(ct, clipfilename) + (int)difftime(time(NULL), start) - lossTime;
            else DrawString(10, 80, (const TCHAR *)"タイマーストップ中", GetColor(0, 0, 0));
            DrawFormatString(10, 50, GetColor(0, 0, 0), (const TCHAR *)"%02d:%02d:%02d\n", tmptime / 3600, tmptime / 60, tmptime % 60);
        }else{
            DrawString(10, 0, (const TCHAR *)"***使い方***\n1,クリスタ上でファイルを開く\n2,チョロっと何か描く\n3,Ctrl+sでタイマー開始", GetColor(0, 0, 0));
        }
        settingButton.draw();
        if(setting)DrawLine(0, 128, 256, 128, GetColor(0, 0, 0));

        if(setting == 0){
            if(settingButton.isClicked()){
                SetWindowSize(256, 256);
                settingButton.reLocate(200, 228);
                settingButton.setString("閉じる");
                setting = 1;
            }
        }else{
            namecheckButton.draw();
            frontcheck.draw();
            activecheck.draw();
            DrawStringToHandle(27, 137, (const TCHAR *)"常に最前面表示", GetColor(0, 0, 0), font_UI);
            DrawStringToHandle(27, 157, (const TCHAR *)"クリスタが非アクティブ時タイマーストップ", GetColor(0, 0, 0), font_UI);
            if(frontcheck.isClicked()){
                frontcheck.reverse();
                settingFlags.frontWindow = frontcheck.getStatus();
                if(frontcheck.getStatus()){
                    SetWindowPos(mainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }else{
                    SetWindowPos(mainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
            }
            if(activecheck.isClicked()){
                activecheck.reverse();
                settingFlags.activeFlag = activecheck.getStatus();
            }
            if(settingButton.isClicked()){
                settingButton.reLocate(200, 100);
                settingButton.setString("設定");
                SetWindowSize(256, 128);
                setting = 0;
            }
            if(namecheckButton.isClicked()){
                if(MessageBox(GetMainWindowHandle(), TEXT("存在しないファイルのタイムを削除します。\nよろしいですか？"), TEXT("確認 : 不要なタイムを削除"), MB_OKCANCEL | MB_ICONWARNING) == IDOK){
                    printf("%s\n", clipstudioDir);
                    nameCheck(ct, clipstudioDir);
                }
            }
        }

        if(wait_stat = WaitForSingleObject(clipStudioProcessHandle, 10)){
            switch(wait_stat){
                case WAIT_FAILED:
                    puts("WaitForSingleObject() failed");
                    exit_flag = 1;
                    break;
                case WAIT_ABANDONED:
                    puts("WaitForSingleObject() returns WAIT_ABANDONED.");
                    exit_flag = 1;
                    break;
                case WAIT_OBJECT_0:
                    puts("Process was finished.");
                    exit_flag = 1;
                case WAIT_TIMEOUT:
                    if((clipfilename = openClipFile(clipstudioDir, 1)) != NULL){
                        if(strcmp(clipfilename, before_filename) != 0){
                            printf("%s has been opened.\n", clipfilename);
                            if(strlen(before_filename) != 0){
                                save_time(ct, before_filename, (int)difftime(time(NULL), start) - lossTime);
                            }
                            lossTime = 0;
                            start = time(NULL);
                            memcpy(before_filename, clipfilename, FILENAME_MAX);
                        }
                        DWORD activeWindowProcessID;
                        GetWindowThreadProcessId(GetForegroundWindow(), &activeWindowProcessID);
                        if(activeWindowProcessID == clipStudioProcessID || settingFlags.activeFlag == false){
                            if(Lossing == 1){
                                lossTime += (int)difftime(time(NULL), beforeLossStart);
                                Lossing = 0;
                            }
                        }else{
                            if(Lossing == 0){
                                beforeLossStart = time(NULL);
                                Lossing = 1;
                            }
                        }
                    }
                    break;
                default:
                    puts("????????????????????");
                    exit_flag = 1;
                    break;
            }
        }else{
            puts("WaitForSingleObject() returns 0.");
            exit_flag = 1;
        }

        ScreenFlip();
    }

    if(strlen(before_filename) != 0) save_time(ct, before_filename, (int)difftime(time(NULL), start) - lossTime);

    for(int i = 0; i < RECORD_MAX; i++){
        if(ct[i].sectime == 0) break;
        printf("%s    Timer : %d\n", ct[i].fileName, ct[i].sectime);
    }

    FILE *txtfp = fopen("CLIPStudio_time_results.txt", "w");
    if(txtfp != NULL){
        for(int i = 0; i < RECORD_MAX; i++){
            if(ct[i].sectime == 0) break;
            fprintf(txtfp, "%s    Timer : %02d:%02d:%02d\n", ct[i].fileName, ct[i].sectime / 3600, ct[i].sectime / 60, ct[i].sectime % 60);
        }
        fclose(txtfp);
    }

    ctfp = fopen("timedata.dat", "wb");
    if(ctfp != NULL){
        fwrite(ct, sizeof(CLIPSTUDIO_TIME), RECORD_MAX, ctfp);
        fclose(ctfp);
    }

    flagsetfp = fopen("flags.bin", "wb");
    if(flagsetfp != NULL){
        fwrite(&settingFlags, sizeof(struct SETTING), 1, flagsetfp);
        fclose(flagsetfp);
    }

    DxLib_End();

    return 0;
}

void save_time(CLIPSTUDIO_TIME *ct, char *name, int time){
    for(int i = 0; i < RECORD_MAX; i++){
        if(strcmp(name, ct[i].fileName) == 0){
            printf("%d %d", ct[i].sectime, time);
            ct[i].sectime += time;
            break;
        }
        if(ct[i].sectime == 0){
            snprintf(ct[i].fileName, FILENAME_MAX, "%s", name);
            ct[i].sectime = time;
            break;
        }
    }
}

int get_starttime(CLIPSTUDIO_TIME *ct, char *name){
    for(int i = 0; i < RECORD_MAX; i++){
        if(strcmp(name, ct[i].fileName) == 0){
            return ct[i].sectime;
        }
    }
    return 0;
}

char *openClipFile(char *folderpath, char init){
    DIR *dir;
	struct dirent *dp;
    static char openingFileName[FILENAME_MAX];
    char tmppath[FILENAME_MAX];
    char deepFolderPath[FILENAME_MAX];
    char *extension;
    static time_t lastSavedFileTime;

    if(init){
        lastSavedFileTime = programStartTime;
        init = 0;
    }

    if((dir = opendir(folderpath)) == NULL){
		fprintf(stderr, "%s cannot open.\n", folderpath);
		return NULL;
	}

    for(dp = readdir(dir); dp != NULL; dp = readdir(dir)){
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)continue;
        if(dp->d_type == DT_DIR){
            snprintf(deepFolderPath, FILENAME_MAX, "%s/%s", folderpath, dp->d_name);
            openClipFile(deepFolderPath, 0);
            snprintf(tmppath, FILENAME_MAX, "%s/%s", dp->d_name, openingFileName);
            memcpy(openingFileName, tmppath, FILENAME_MAX);
        }else{
            if((extension = get_extension(dp->d_name)) != NULL){
                if(strcmp(extension, ".clip") == 0){
                    struct _stat st;
                    snprintf(tmppath, FILENAME_MAX, "%s/%s", folderpath, dp->d_name);
                    _stat(tmppath, &st);
                    //printf("%s    update : %s", dp->d_name, asctime(localtime(&st.st_atime)));
                    if(difftime(lastSavedFileTime, st.st_atime) < 0){
                        lastSavedFileTime = st.st_atime;
                        snprintf(openingFileName, FILENAME_MAX, "%s", dp->d_name);
                    }
                }
            }
        }
    }
    closedir(dir);
    if(lastSavedFileTime == programStartTime)return NULL;
    else return openingFileName;
}

void nameCheck(CLIPSTUDIO_TIME *ct, char *folderpath){
    DIR *dir;
	struct dirent *dp;
    std::stack<DIR *> dir_stack;
    std::vector<std::string> name_vector;
    char folderPathBuffer[FILENAME_MAX];
    char tmp_folderPathBuffer[FILENAME_MAX];
    char tmp_namebuf[FILENAME_MAX];
    char fileExist;

    if((dir = opendir(folderpath)) == NULL){
        fprintf(stderr, "%s cannot open.\n", folderpath);
        return;
    }

    for(int i = 0; ct[i].sectime != 0 && i < RECORD_MAX; i++){
        fileExist = 0;
        seekdir(dir, 0);
        memcpy(folderPathBuffer, folderpath, FILENAME_MAX);
        for(dp = readdir(dir);; dp = readdir(dir)){
            if(dp == NULL){
                if(dir_stack.empty()){
                    break;
                }else{
                    closedir(dir);
                    dir = dir_stack.top();
                    dir_stack.pop();
                    name_vector.pop_back();
                    continue;
                }
            }
            printf("entry : %s\n", dp->d_name);
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)continue;
            if(dp->d_type == DT_DIR){
                printf("folder : %s\n", dp->d_name);
                dir_stack.push(dir);
                name_vector.push_back(std::string(dp->d_name));
                snprintf(tmp_folderPathBuffer, FILENAME_MAX, "%s/%s", folderPathBuffer, dp->d_name);
                memcpy(folderPathBuffer, tmp_folderPathBuffer, FILENAME_MAX);
                if((dir = opendir(folderPathBuffer)) == NULL){
                    fprintf(stderr, "%s cannot open.\n", folderPathBuffer);
                    return;
                }
            }else{
                printf("file : %s\n", dp->d_name);
                memset(tmp_namebuf, (int)'\0', FILENAME_MAX);
                for(int j = 0; j < name_vector.size(); j++){
                    strncat(tmp_namebuf, name_vector.at(j).c_str(), FILENAME_MAX);
                    strncat(tmp_namebuf, "/", 2);
                }
                strncat(tmp_namebuf, dp->d_name, FILENAME_MAX);
                printf("%s   %s\n", ct[i].fileName, tmp_namebuf);
                if(strcmp(ct[i].fileName, tmp_namebuf) == 0){
                    fileExist = 1;
                }
            }
        }
        if(fileExist == 0){
            for(int j = i; j < RECORD_MAX - 1; j++){
                memcpy(ct[j].fileName, ct[j + 1].fileName, FILENAME_MAX);
                ct[j].sectime = ct[j + 1].sectime;
            }
            i--;
        }else{
            printf("%s exist\n", ct[i].fileName);
        }
    }
    closedir(dir);
}

char *get_extension(char *filename){
	int i;
	char *next_dot;
	for(i = 1; filename[i] != '\0'; i++){
		if(filename[i] == '.'){
			if((next_dot = get_extension(filename + i)) != NULL)return next_dot;
			else return filename + i;
		}
	}
	return NULL;
}

