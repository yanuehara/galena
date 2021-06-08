#include "singletonLogger.h"

// Define the static Singleton pointer
SingletonLogger* SingletonLogger::inst_ = nullptr;

SingletonLogger::SingletonLogger(std::string filename){
    this->logfile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
}

SingletonLogger* SingletonLogger::getInstance(std::string filename) {
   if (inst_ == nullptr) {
      inst_ = new SingletonLogger(filename);
   }
   return(inst_);
}

void SingletonLogger::writeEntry(std::string entry){
    //this->logfile.write(entry.c_str(), sizeof(char)*entry.size());
    this->logfile << entry;
    //std::clog << "Wrote to file" << std::endl;
}