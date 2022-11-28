
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <fileapi.h>

using namespace std;
const int factor = 16;
const int overlapp = 32;

void menu();
void diskList();
void diskType(LPTSTR c);
void diskListInfo();
void systemFlags(DWORD flags);
void createFolder();
void deleteFolder();
void createFile();
void copyFile();
void moveFile();
void copyFileAsinc();
void desypherFlags(DWORD flags);
void fileInfo();
void fillTime(SYSTEMTIME* time);
void _stdcall afterReadRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props);
void _stdcall afterWriteRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props);


typedef union {
    unsigned long long total;
    struct {
        DWORD last;
        DWORD next;
    };
} sizeFile;

typedef struct {
    void* buffer;
    sizeFile currentOffset;
} threadProps;

sizeFile fileSize, clSize;
HANDLE flag1, flag2;

int main()
{
    int c;
    menu();
    cin >> c;
    while (c) {
        switch (c)
        {
        case 0:
            break;
        case 1:
            diskList();
            break;
        case 2:
            createFolder();
            break;
        case 3:
            deleteFolder();
            break;
        case 4:
            createFile();
            break;
        case 5:
            moveFile();
            break;
        case 6:
            copyFile();
            break;
        case 7:
            copyFileAsinc();
            break;
        case 8:
            fileInfo();
            break;
        default:
            menu();
        }
        //cout << "\033[2J\033[1;1H"; // очистка экрана и перевод курсора в левый верхний угол
        menu();
        cin >> c;
    }

}

void menu() {
    cout << "\n0 - exit";
    cout << "\n1 - disk listing";
    cout << "\n2 - create a new folder ";
    cout << "\n3 - delete a folder ";
    cout << "\n4 - create a new file ";
    cout << "\n5 - move a new file ";
    cout << "\n6 - copy a new file ";
    cout << "\n7 - copy file asinc";
    cout << "\n8 - get information about file attributes";
    cout << "\nYour choice: ";
}

void diskList() {
    int j = 0;
    int c;
    DWORD drives = GetLogicalDrives();

    cout << "Dist list: ";
    for (DWORD i = 1; j < sizeof(DWORD) * 8; i <<= 1, ++j) {
        if (drives & i)
            cout << char('A' + j) << " ";
    }
    cout << "\nIf you want to get information about a certain disk, ENTER 1. Otherwise, any number\nYour choice: ";
    cin >> c;
    switch (c)
    {
    case 1:
        diskListInfo();
        break;
    default:
        break;
    }
};

void diskType(LPTSTR c) {
    UINT diskType = GetDriveType(c);
    switch (diskType) {
    case DRIVE_UNKNOWN:
        cout << "DRIVE_UNKNOWN";// неизвестен
        break;
    case DRIVE_NO_ROOT_DIR:
        cout << "DRIVE_NO_ROOT_DIR";// нет :-(((
        break;
    case DRIVE_REMOVABLE:
        cout << "DRIVE_REMOVABLE";// извлекаемый диск
        break;
    case DRIVE_FIXED:
        cout << "DRIVE_FIXED";// нормальный жеский диск
        break;
    case DRIVE_REMOTE:
        cout << "DRIVE_REMOTE";// удаленный, т.е. сетевой
        break;
    case DRIVE_CDROM:
        cout << "DRIVE_CDROM";// СD ROM
        break;
    case DRIVE_RAMDISK:
        cout << "DRIVE_RAMDISK";// В оперативной памяти
        break;
    default:
        cout << "\n";
    }
};

void diskListInfo() {
    wchar_t c;
    const DWORD VolumeNameSize = 50, FileSystemNameSize = 10;
    DWORD VolumeSerialNumber, MaximumComponentLength;
    DWORD SectorsPerCluster; //число секторов в кластере
    DWORD BytesPerSector; //число байт в секторе
    DWORD NumberOfFreeClusters; //число свободных кластеров
    DWORD TotalNumberOfClusters; //общее число кластеров
    DWORD FileSF; //Указатель на переменную, которая получает флаги
    wchar_t  VolumeNameBuffer[VolumeNameSize], FileSystemNameBuffer[FileSystemNameSize];
    wchar_t PathName[] = L"C:\\";
    cout << "Enter disc name: ";
    wcin >> c;
    PathName[0] = c;
    diskType(PathName);
    GetVolumeInformationW(PathName, VolumeNameBuffer, VolumeNameSize, &VolumeSerialNumber, &MaximumComponentLength, &FileSF, FileSystemNameBuffer, FileSystemNameSize);
    cout << "\nDrive name: " << VolumeNameBuffer;
    cout << "\nSerial number: " << VolumeSerialNumber;
    cout << "\nMax file name length: " << MaximumComponentLength;
    cout << "\nFile system: " << FileSystemNameBuffer;
    GetDiskFreeSpace(PathName, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
    cout << "\nTotal number of clusters: " << TotalNumberOfClusters;
    cout << "\nNumber of free clusters: " << NumberOfFreeClusters;
    cout << "\nNumber of sectors in the cluster: " << SectorsPerCluster;
    cout << "\nNumber of bytes in the sector: " << BytesPerSector << '\n';
    systemFlags(FileSF);
    cout << "\n";
}

void systemFlags(DWORD FileSF) {
    if (FileSF & FILE_CASE_SENSITIVE_SEARCH)
        cout << "\nThe specified volume supports case-sensitive file names.";
    if (FileSF & FILE_CASE_PRESERVED_NAMES)
        cout << "\nThe specified volume supports preserved case of file names when it places a name on disk";
    if (FileSF & FILE_UNICODE_ON_DISK)
        cout << "\nThe specified volume supports Unicode in file names as they appear on disk";
    if (FileSF & FILE_PERSISTENT_ACLS)
        cout << "\nThe specified volume preserves and enforces access control lists (ACL)";
    if (FileSF & FILE_FILE_COMPRESSION)
        cout << "\nThe specified volume supports file-based compression";
    if (FileSF & FILE_VOLUME_QUOTAS)
        cout << "\nThe specified volume supports disk quotas";
    if (FileSF & FILE_SUPPORTS_SPARSE_FILES)
        cout << "\nThe specified volume supports sparse files";
    if (FileSF & FILE_SUPPORTS_REPARSE_POINTS)
        cout << "\nThe specified volume supports reparse points";
    if (FileSF & FILE_SUPPORTS_REMOTE_STORAGE)
        cout << "\nThe file system supports remote storage";
    if (FileSF & FILE_RETURNS_CLEANUP_RESULT_INFO)
        cout << "\nThe file system returns information that describes additional actions taken during cleanup";
    if (FileSF & FILE_SUPPORTS_POSIX_UNLINK_RENAME)
        cout << "\nThe file system supports POSIX-style delete and rename operations";
    if (FileSF & FILE_VOLUME_IS_COMPRESSED)
        cout << "\nThe specified volume is a compressed volume";
    if (FileSF & FILE_SUPPORTS_OBJECT_IDS)
        cout << "\nThe file system supports object identifiers";
    if (FileSF & FILE_SUPPORTS_ENCRYPTION)
        cout << "\nThe file system supports encryption";
    if (FileSF & FILE_NAMED_STREAMS)
        cout << "\nThe file system supports named data streams for a file";
    if (FileSF & FILE_READ_ONLY_VOLUME)
        cout << "\nThe specified volume is read-only";
    if (FileSF & FILE_SEQUENTIAL_WRITE_ONCE)
        cout << "\nThe specified volume can be written to one time only";
    if (FileSF & FILE_SUPPORTS_TRANSACTIONS)
        cout << "\nThe file system supports transaction processing";
    if (FileSF & FILE_SUPPORTS_HARD_LINKS)
        cout << "\nThe file system supports direct links to other devices and partitions";
    if (FileSF & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
        cout << "\nThe file system supports extended attributes (EAs)";
    if (FileSF & FILE_SUPPORTS_OPEN_BY_FILE_ID)
        cout << "\nThe file system supports open by file ID";
    if (FileSF & FILE_SUPPORTS_USN_JOURNAL)
        cout << "\nThe file system supports update sequence number (USN) journals";
    if (FileSF & FILE_SUPPORTS_INTEGRITY_STREAMS)
        cout << "\nThe file system supports integrity streams";
    if (FileSF & FILE_SUPPORTS_BLOCK_REFCOUNTING)
        cout << "\nThe file system supports block cloning, that is, sharing logical clusters between files on the same volume";
    if (FileSF & FILE_SUPPORTS_SPARSE_VDL)
        cout << "\nThe file system tracks whether each cluster of a file contains valid data or invalid data";
    if (FileSF & FILE_DAX_VOLUME)
        cout << "\nThe specified volume is a direct access (DAX) volume";
    if (FileSF & FILE_SUPPORTS_GHOSTING)
        cout << "\nThe specified volume supports ghosting";
};

void createFolder() {
    wchar_t fullPath[255];
    cout << "A folder will be created in ConsoleApplication\ConsoleApplication\nEnter folder name: ";
    wcin.ignore(INT_MAX, '\n');
    wcin.getline(fullPath, 255, L'\n');
    if (CreateDirectory(fullPath, NULL)) {
        cout << "Folder created successfully\n";
    }
    else {
        cout << "An error has occurred! Try again.\n";
        createFolder();
    }
};

void deleteFolder() {
    wchar_t fullPath[255];
    cout << "Enter the name of the folder you want to delete: ";
    wcin.ignore(INT_MAX, '\n');
    wcin.getline(fullPath, 255, L'\n');
    if (RemoveDirectory(fullPath)) {
        cout << "Folder delete successfully\n";
    }
    else {
        cout << "An error has occurred! Try again.\n";
        deleteFolder();
    }
};

void createFile() {
    char access;
    wchar_t fullPath[255];
    DWORD DesiredAccess, ShareMode = 0;
    HANDLE flag;

    cout << "A file will be created in ConsoleApplication\ConsoleApplication\nEnter file name: ";
    wcin.ignore(INT_MAX, '\n');
    wcin.getline(fullPath, 255, L'\n');
    cout << "Specify access:\nr - GENERIC_READ; \nw - GENERIC_WRITE;\nrw - both\n";
    cin >> access;
    switch (access)
    {
    case 'r':
        DesiredAccess = GENERIC_READ;
        break;
    case 'w':
        DesiredAccess = GENERIC_WRITE;
        break;
    case 'rw':
        DesiredAccess = GENERIC_READ | GENERIC_WRITE;;
        break;
    default:
        cout << "Try again";
        createFile();
    }
    wcin.ignore(INT_MAX, '\n');
    cout << "Access mode:\nr-read access\nd-access to delete\nw-write access\n";
    cin >> access;
    switch (access)
    {
    case 'r':
        DesiredAccess |= FILE_SHARE_READ;
        break;
    case 'd':
        DesiredAccess |= FILE_SHARE_DELETE;
        break;
    case 'w':
        DesiredAccess |= FILE_SHARE_WRITE;
        break;
    default:
        cout << "Try again";
        createFile();
    }
    wcin.ignore(INT_MAX, '\n');

    flag = CreateFile(fullPath, DesiredAccess, ShareMode, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (flag == INVALID_HANDLE_VALUE) {
        cout << "An error has occurred! Try again.\n";
        createFile();
    }
    if (flag != INVALID_HANDLE_VALUE) cout << "Folder created successfully\n";

    if (GetLastError() == ERROR_FILE_EXISTS) {
        cout << "An error has occurred!\nA file with the same name has already been created! Try again.\n";
        createFile();
    }
};

void copyFile() {
    wchar_t file[255], fileCopy[255];
    wcin.ignore(INT_MAX, '\n');
    cout << "Enter the name of the file to be copy: ";
    wcin.getline(file, 255, L'\n');
    wcin.ignore(INT_MAX, '\n');
    cout << "Enter the name of the copied file: ";
    wcin.getline(fileCopy, 255, L'\n');
    if (CopyFile(file, fileCopy, true))
        cout << "Copying was successful!" << std::endl;
    else if (GetLastError() == 80L) {
        cout << "An error has occurred!\nA file with the same name has already been created! Try again.\n";
        copyFile();
    }
    else {
        cout << "An error has occurred! Try again.\n";
        copyFile();
    }
};

void moveFile() {
    wchar_t file[255], fileMove[255];
    wcin.ignore(INT_MAX, '\n');
    cout << "Enter the name of the file to be moved: ";
    wcin.getline(file, 255, L'\n');
    wcin.ignore(INT_MAX, '\n');
    cout << "Enter a new path. You can change the filename: ";
    wcin.getline(fileMove, 255, L'\n');
    if (MoveFile(file, fileMove))
        cout << "Moving was successful!!\n";
    else if (GetLastError() == 183L) {
        cout << "An error has occurred!\nA file with the same name has already been created! Try again.\n";
        moveFile();
    }
    else {
        cout << "An error has occurred! Try again.\n";
        moveFile();
    }
}
void _stdcall afterReadRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props) {
    threadProps* tprop = static_cast<threadProps*>(props->hEvent);
    WriteFileEx(flag2, tprop->buffer, bytesTransfered, props, afterWriteRoutine);
};

void _stdcall afterWriteRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props) {
    threadProps* tprop = static_cast<threadProps*>(props->hEvent);

    tprop->currentOffset.total += clSize.total * factor;
    props->Offset = tprop->currentOffset.last;
    props->OffsetHigh = tprop->currentOffset.next;

    if (fileSize.total > tprop->currentOffset.total) 
        ReadFileEx(flag1, tprop->buffer, clSize.total, props, afterReadRoutine);
}

void copyFileAsinc() {
    wchar_t file1[MAX_PATH], file2[MAX_PATH], lpRootPathName[] = L"C:\\";

    threadProps threadProps[overlapp];
    OVERLAPPED props[overlapp];

    DWORD lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters, startTime, endTime;
     
    
    wcin.ignore(INT_MAX, '\n');
    cout << "Enter the path of the copied file: ";
    wcin.getline(file1, MAX_PATH, L'\n');

    cout << "Enter the final path: ";
    wcin.getline(file2, MAX_PATH, L'\n');

    flag1 = CreateFile(file1,
        FILE_GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
        NULL);
    flag2 = CreateFile(file2,
        FILE_GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        CREATE_NEW,
        FILE_FLAG_OVERLAPPED,
        NULL);

    if (flag1 == INVALID_HANDLE_VALUE || flag2 == INVALID_HANDLE_VALUE) {
        cout << "An error has occurred! Try again.\n";
        return;
    }

    fileSize.last = GetFileSize(flag1, &fileSize.next);

    lpRootPathName[0] = file1[0];
    GetDiskFreeSpace(lpRootPathName, &lpSectorsPerCluster, &lpBytesPerSector, &lpNumberOfFreeClusters, &lpTotalNumberOfClusters);
    clSize.total = lpSectorsPerCluster * lpBytesPerSector * factor;

    startTime = GetTickCount();

    for (int i = 0; i < overlapp; ++i) {
        threadProps[i].buffer = malloc(clSize.total + 1);
        threadProps[i].currentOffset.total = i * clSize.total;

        props[i].Internal = 0;
        props[i].InternalHigh = 0;
        props[i].Pointer = 0;
        props[i].Offset = threadProps[i].currentOffset.last;
        props[i].OffsetHigh = threadProps[i].currentOffset.next;

        props[i].hEvent = threadProps + i;

        ReadFileEx(flag1,
            threadProps[i].buffer,
            clSize.total,
            props + i,
            afterReadRoutine);
    }

    while (SleepEx(100, true));

    endTime = GetTickCount();
    cout << "Copying was successful! " << endTime - startTime;

    CloseHandle(flag1);
    CloseHandle(flag2);
    for (int i = 0; i < overlapp; ++i) free(threadProps[i].buffer);
}



void desypherFlags(DWORD flags) {

    if (flags & FILE_ATTRIBUTE_READONLY) cout << "file is readonly;\n";
    if (flags & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) cout << "file is not fully present locally;\n";
    if (flags & FILE_ATTRIBUTE_RECALL_ON_OPEN) cout << "file is virtual;\n";
    if (flags & FILE_ATTRIBUTE_REPARSE_POINT)cout << "file is a symbolic link;\n";
    if (flags & FILE_ATTRIBUTE_SPARSE_FILE) cout << "file is sparse;\n";
    if (flags & FILE_ATTRIBUTE_SYSTEM) cout << "system uses only a part of this file;\n";
    if (flags & FILE_ATTRIBUTE_TEMPORARY) cout << "file is being used for temporary storage;\n";
    if (flags & FILE_ATTRIBUTE_VIRTUAL) cout << "file is reserved for system use;\n";
    if (flags & FILE_ATTRIBUTE_PINNED) cout << "file should be kept fully present locally even when not being actively accessed;\n";
    if (flags & FILE_ATTRIBUTE_UNPINNED) cout << "file should not be kept fully present locally except when being actively accessed;\n";
    if (flags & FILE_ATTRIBUTE_ARCHIVE) cout << "file is an archive;\n";
    if (flags & FILE_ATTRIBUTE_COMPRESSED) cout << "file is compressed;\n";
    if (flags & FILE_ATTRIBUTE_DEVICE) cout << "file is reserved for system use;\n";
    if (flags & FILE_ATTRIBUTE_DIRECTORY) cout << "file is a directory;\n";
    if (flags & FILE_ATTRIBUTE_ENCRYPTED) cout << "file is encrypted;\n";
    if (flags & FILE_ATTRIBUTE_HIDDEN) cout << "file is hidden;\n";
    if (flags & FILE_ATTRIBUTE_INTEGRITY_STREAM) cout << "file is configured with integrity;\n";
    if (flags & FILE_ATTRIBUTE_NORMAL) cout << "file has no special attributes;\n";
    if (flags & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) cout << "file not to be indexed by the content indexing service;\n";
    if (flags & FILE_ATTRIBUTE_NO_SCRUB_DATA) cout << "file is not to be read by the background data integrity scanner;\n";
    if (flags & FILE_ATTRIBUTE_OFFLINE) cout << "file is not available immediately;\n";
}

void fileInfo() {
    int c;
    wchar_t fullPath[255];
    DWORD num;
    HANDLE flag;
    _BY_HANDLE_FILE_INFORMATION info;
    FILETIME fileTime;
    SYSTEMTIME time;
    std::cin.ignore(INT_MAX, '\n');
    cout << "Enter the name of the file: ";
    wcin.getline(fullPath, 255, L'\n');
    flag = CreateFile(fullPath, GENERIC_READ | FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (flag == INVALID_HANDLE_VALUE) {
        cout << "An error has occurred! Try again.\n";
        fileInfo();
    }
    if (!GetFileInformationByHandle(flag, &info)) { 
        cout << "An error has occurred! Try again." << std::endl;
        fileInfo;
    }
    desypherFlags(info.dwFileAttributes);
    FileTimeToSystemTime(&(info.ftCreationTime), &time);
    cout << "File created: " << time.wDay << "." << time.wMonth << "." << time.wYear
        << ", " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " UTC 0,\n";
    FileTimeToSystemTime(&(info.ftLastAccessTime), &time);
    cout << "ПLast call: " << time.wDay << "." << time.wMonth << "." << time.wYear
        << ", " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " UTC 0,\n";
    FileTimeToSystemTime(&(info.ftLastWriteTime), &time);
    cout << "Last modified: " << time.wDay << "." << time.wMonth << "." << time.wYear
        << ", " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " UTC 0,\n";
    cout << "Serial number: " << info.dwVolumeSerialNumber;
    cout << ",\nSize: " << info.nFileSizeHigh << info.nFileSizeLow;
    cout << " Bytes,\nNumber of links: " << info.nNumberOfLinks;
    cout << ",\nFile ID: " << info.nFileIndexHigh << info.nFileIndexLow;
    cout << "\nIf you want to change file flags enter 1\nYour choice: ";
    cin >> c;
    if (c == 1) {
        cout << "Enter number of flags: ";
        wcin >> num;
        if (SetFileAttributes(fullPath, num))
            cout << "The operation was successful!\n";
        else {
            cout << "An error has occurred!";
        }
    }
    std::cin.ignore(INT_MAX, '\n');
    cout << "\nIf you want to change time creating enter 2\nYour choice: ";
    cin >> c;
    if (c == 2) {
        fillTime(&time);
        SystemTimeToFileTime(&time, &fileTime);
        if (SetFileTime(flag, &fileTime, NULL, NULL))
            cout << "The operation was successful!\n";
        else {
            cout << "An error has occurred!";
        }
    }
    std::cin.ignore(INT_MAX, '\n');
    cout << "\nIf you want to change time editing enter 3\nYour choice: ";
    cin >> c;
    if (c == 3) {
        fillTime(&time);
        SystemTimeToFileTime(&time, &fileTime);
        if (SetFileTime(flag, NULL, &fileTime, NULL))
            cout << "The operation was successful!\n";
        else {
            cout << "An error has occurred! Try again.\n";
        }
    }
    std::cin.ignore(INT_MAX, '\n');
    CloseHandle(flag);

};

void fillTime(SYSTEMTIME* dataTime) {
    WORD value;

    cout << "Enter year ";
    wcin >> value;
    wcin.ignore(INT_MAX, '\n');
    dataTime->wYear = value;
    cout << "Enter mounth (number): ";
    wcin >> value;
    wcin.ignore(INT_MAX, '\n');
    dataTime->wMonth = value;
    cout << "Enter day: ";
    wcin >> value;
    wcin.ignore(INT_MAX, '\n');
    dataTime->wDay = value;
    cout << "Enter hour: ";
    wcin >> value;
    wcin.ignore(INT_MAX, '\n');
    dataTime->wHour = value;
    cout << "Enter minutes: ";
    wcin >> value;
    wcin.ignore(INT_MAX, '\n');
    dataTime->wMinute = value;
    cout << "Enter second: ";
    wcin >> value;
    wcin.ignore(INT_MAX, '\n');
    dataTime->wSecond = value;
}