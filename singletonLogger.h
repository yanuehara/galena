#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class SingletonLogger {
    public:
        static SingletonLogger* getInstance(std::string filename);
        static SingletonLogger* getInstance();
        void writeEntry(std::string entry);
        void closeLog();

    protected:
        bool logfileOpened {false};
        std::ofstream logfile;
    

    private:
        static SingletonLogger* inst_;
        SingletonLogger(std::string filename);
        
        //Delete
        SingletonLogger() = delete;
        SingletonLogger(const SingletonLogger&) = delete;
        SingletonLogger& operator=(const SingletonLogger&) = delete;
        SingletonLogger(SingletonLogger&&) = delete;
        SingletonLogger& operator=(SingletonLogger&&) = delete;
};
