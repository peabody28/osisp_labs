#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <functional>
#include "file-struct.h"

using namespace std;

struct record_s getRecordByNumber(int fd, int number)
{
    struct record_s record;
    size_t size = sizeof(struct record_s);

    int currentCursorPos = lseek(fd, 0, SEEK_CUR);
    lseek(fd, size*(number - 1), 0);

    char* buffer = new char[size];
    ssize_t bytesRead = read(fd, buffer, size);
    memcpy(&record, buffer, size);

    lseek(fd, currentCursorPos, 0);

    return record;
}

void putRecordByNumber(struct record_s record, int fd, int number)
{
    size_t size = sizeof(struct record_s);

    int currentCursorPos = lseek(fd, 0, SEEK_CUR);
    lseek(fd, size*(number - 1), 0);
    
    char* buffer = new char[size];
    memcpy(buffer, &record, size);

    write(fd, buffer, size);

    lseek(fd, currentCursorPos, 0);
}

void printRecord(struct record_s record)
{
    cout << "name:" << record.name << endl;
    cout << "address:" << record.address << endl;
    cout << "semestr:" << record.semester << endl;
}

void lockFile(struct flock* lock, int fd)
{
    lock->l_type = F_WRLCK;

    /* Установка блокировки */
    cout << "start locking" << endl;

    fcntl(fd, F_SETLKW, lock);

    cout << "is locked (press any key)" << endl;
}

void unlockFile(struct flock* lock, int fd)
{
    lock->l_type = F_UNLCK;

    fcntl(fd, F_SETLKW, lock);

    cout << "unlocked" << endl;
}

void lockAction(struct flock* lock, int fd, const std::function<void()>& f)
{
    lockFile(lock, fd);

    f();

    unlockFile(lock, fd);
}

void appendSemestr(struct record_s* record)
{
    record->semester++;
}

int getActionCodeFromConsole()
{
    int actionCode;
    cout << endl;
    cout << "input action code:"<< endl;
    cout << "1 - select record by id"<< endl;
    cout << "2 - show record"<< endl;
    cout << "3 - append semestr to record"<< endl;
    cout << "4 - save record"<< endl;
    cout << "5 - show all records" << endl;
    cout << "other_key - exit" << endl;

    cin >> actionCode;
    system("clear");
    return actionCode;
}

int main(int argc, char* argv[])
{
    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    char* filename = argv[1];
    int fd = open(filename, O_RDWR);
    cout << "opened " << filename << " " << fd << endl;

    struct record_s record;
    int recordNumber;

    while(true)
    {
        int code = getActionCodeFromConsole();
        if(code == 1)
        {
            lockAction(&lock, fd, [&]
            {
                // ввод помещен тут, чтоб наглядно показать блокировку (по-хорошему он должен быть вне блокировки)
                cout << "input record number:";
                cin >> recordNumber;

                record = getRecordByNumber(fd, recordNumber);
            });
        }
        else if(code == 2)
            printRecord(record);
        else if(code == 3)
            appendSemestr(&record);
        else if(code == 4)
        {
            lockAction(&lock, fd, [&]{
                putRecordByNumber(record, fd, recordNumber);
            });
        }
        else
            break;
    }

    close(fd);

    return 0;
}