#include <iostream>
#include <thread>
#include <algorithm>
//#include <vector>
#include <fstream> 
#include <stdexcept>
#include <fstream>
#include <future>

using namespace std;

//сортировка половины (части) большого файла и запись в кучу маленьких отсортированных файлов
int sortarr(int start, int fin, int th_num, const std::string filename)
{
    if (start == fin)
        return -1;
    int bufsize = 1024;
    int readed = 0;
    std::string outname("");
    int writed = 0; // files created
    uint64_t buf[bufsize];
    std::ifstream file(filename.c_str(), std::ifstream::in | std::ifstream::binary);

    if (!file.is_open()) 
    {
        throw std::runtime_error("Open error");
    }

    file.seekg(start);
    int pos = file.tellg();
    while (pos < fin) 
    {
        if (fin - pos > bufsize) 
        {
            file.read(reinterpret_cast<char*>(buf), bufsize);
            readed = file.gcount();
            if (readed < bufsize) 
                throw std::runtime_error("Read error-1");
        }
        else
        {
            file.read(reinterpret_cast<char*>(buf), (fin-pos));
            readed = file.gcount();
            if (readed < fin-pos) 
                throw std::runtime_error("Read error-2");
        }
        pos += readed;
        std::sort(buf, buf+readed/8);
        outname = std::to_string(th_num) + "-" + std::to_string(writed);
        std::ofstream out(outname, std::ofstream::out | std::ofstream::binary);

        if (!out.is_open()) 
            throw std::runtime_error("Open error");

        out.write(reinterpret_cast<char*>(buf), readed);
        out.close();
        writed += 1;
    }
    return writed-1; // номер последнего записанного файла
}

void my_merge(const string filename1, const string filename2, const string outname) 
{
    int bufsize = 1024;
    uint64_t buf1[bufsize];
    int done1 = 0, readed1 = 0;
    bool end1 = false;
    uint64_t buf2[bufsize];
    int done2 = 0, readed2 = 0;
    bool end2 = false;

    int outbufsize = bufsize * 2;
    uint64_t bufout[outbufsize];
    int doneout = 0;
    
    std::ifstream file1(filename1, std::ifstream::in | std::ifstream::binary);
    if (!file1.is_open()) 
        throw std::runtime_error("Open error");

    std::ifstream file2(filename2, std::ifstream::in | std::ifstream::binary);
    if (!file2.is_open()) 
        throw std::runtime_error("Open error");

    std::ofstream out(outname, std::ofstream::out | std::ofstream::binary);
    if (!out.is_open()) 
        throw std::runtime_error("Open error");

    while (true) 
    {
        // читаем в нужный буфер
        if (done1 == readed1)
        {
            file1.read(reinterpret_cast<char*>(buf1), bufsize*sizeof(uint64_t));
            readed1 = file1.gcount();
            done1 = 0;
            if (readed1 == 0)
                end1 = true;
            readed1 /= 8;
        }
        if (done2 == readed2)
        {
            file2.read(reinterpret_cast<char*>(buf2), bufsize*sizeof(uint64_t));
            readed2 = file2.gcount();
            done2 = 0;
            if (readed2 == 0)
                end2 = true;
            readed2 /= 8;
        }
        // выводим накопившиеся в выходном буфере данные
        out.write(reinterpret_cast<char*>(bufout), doneout*sizeof(uint64_t));
        doneout = 0;
        // выход - оба файла закончились
        if (end1 && end2)
            return;
        // слияние
        while ((done1 < readed1) && (done2 < readed2))
        {
            if (buf1[done1] < buf2[done2])
            {
                bufout[doneout] = buf1[done1];
                doneout++;
                done1++;
            }
            else
            {
                bufout[doneout] = buf2[done2];
                doneout++;
                done2++;
            }
        }
        if (end1)
            while (done2 < readed2)
            {
                bufout[doneout] = buf2[done2];
                doneout++;
                done2++;
            }
        if (end2)
            while (done1 < readed1)
            {
                bufout[doneout] = buf1[done1];
                doneout++;
                done1++;
            }
    }
} 

void many2one(int th_num, int count)
{
    if (count <= 1)
        return;
    int done = 0;
    string file1 = std::to_string(th_num) + "-" + std::to_string(done);
    string file2 = std::to_string(th_num) + "-" + std::to_string(done+1);
    string out = "out" + std::to_string(th_num) + "tmp";
    while (done + 1 <= count)
    {
        my_merge(file1, file2, out);
        remove(file1.c_str());
        remove(file2.c_str());
        rename(out.c_str(), file1.c_str());
        done++;
        file2 = std::to_string(th_num) + "-" + std::to_string(done+1);
    }
}

int64_t file_size(const std::string filename) 
{
    std::ifstream file(filename.c_str(), std::ifstream::in | std::ifstream::binary);

    if (!file.is_open())
        return -1;

    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.close();

    return size;
}

// на вход - имя файла к сортировке и имя выходного файла
int main(int argc, char* argv[])
{
    if (argc != 3)
        return -1;
    int size = file_size(argv[1]);
    int size1 = size/2;

    if ((size1 % 4) != 0)
    {
        size1 = size1 - (size1 % 4);
    }

    try {

        auto one = async(sortarr, 0, size1, 1, argv[1]);
        int result1 = one.get();  // номер последнего записанного файла

        auto two = async(sortarr, size1, size, 2, argv[1]);
        int result2 = two.get();  // номер последнего записанного файла

        thread first(many2one, 1, result1);
        thread second(many2one, 2, result2);

        first.join();
        second.join();

        string file1 = "1-0";
        string file2 = "2-0";


        my_merge(file1, file2, argv[2]);

        remove(file1.c_str());
        remove(file2.c_str());
    }
    catch (runtime_error &s) {
        cerr << s.what() << endl;
        return 1;
    }

    return 0; 
}
