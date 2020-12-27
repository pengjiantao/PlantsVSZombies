//
// Created by fiyqkrc on 12/27/20.
//

#ifndef INFOREAD_INFOREAD_H
#define INFOREAD_INFOREAD_H
#include<fstream>
#include "Info.h"
#include <vector>
class InfoRead {
public:
    InfoRead(const std::string& path,const std::string& mode);
    ~InfoRead();
    void setFile(const std::string& path,const std::string& mode);
    void closeFile();
    Info getInfo();
    std::vector<Info> getInfos();
    void writeInfo(const Info& s);
    void writeInfos(const std::vector<Info>& s);
    bool empty();
private:
    static Info understandInfo(const std::string& s);
    std::ifstream file_in_;
    std::ofstream file_out_;
    std::string mode_;
};


#endif //INFOREAD_INFOREAD_H
