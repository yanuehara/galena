#include "singletonLogger.h"

// Define the static Singleton pointer
SingletonLogger* SingletonLogger::inst_ = nullptr;

SingletonLogger::SingletonLogger(std::string filename){
    this->logfile.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
}

SingletonLogger* SingletonLogger::getInstance(std::string filename) {
   if (inst_ == nullptr) {
      inst_ = new SingletonLogger(filename);
   }
   return(inst_);
}

SingletonLogger* SingletonLogger::getInstance() {
   if (inst_ == nullptr) {
      throw std::runtime_error("Instance not initialized");
   }
   return(inst_);
}

void SingletonLogger::writeEntry(std::string entry){
    //this->logfile.write(entry.c_str(), sizeof(char)*entry.size());
    this->logfile << entry << std::endl;
    //std::clog << "Wrote to file" << std::endl;
    this->logfile.flush();
}

void SingletonLogger::closeLog(){
    this->logfile.close();
}