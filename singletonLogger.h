#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class SingletonLogger {
    public:
        static SingletonLogger* getInstance(std::string filename);
        void writeEntry(std::string entry);

    protected:
        bool logfileOpened {false};
        std::ofstream logfile;
    

    private:
        static SingletonLogger* inst_;
        SingletonLogger(std::string filename);
        
        //Delete
        SingletonLogger(const SingletonLogger&) = delete;
        SingletonLogger& operator=(const SingletonLogger&) = delete;
        SingletonLogger(SingletonLogger&&) = delete;
        SingletonLogger& operator=(SingletonLogger&&) = delete;
};