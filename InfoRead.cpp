//
// Created by fiyqkrc on 12/27/20.
//

#include "InfoRead.h"
#include <fstream>


bool InfoRead::empty() {
    if(mode_.empty()||(mode_=="r"&&!file_in_)||(mode_=="w"&&!file_out_))
        return true;
    else
        return false;
}

InfoRead::~InfoRead() {
    if(mode_=="r")
        file_in_.close();
    if(mode_=="w")
        file_out_.close();
}

InfoRead::InfoRead(const std::string &path, const std::string &mode) {
    mode_=mode;
    if(mode=="r")
        file_in_.open(path,std::ios::in);
    else if(mode=="w")
        file_out_.open(path,std::ios::out);
    else
        mode_="";
}

void InfoRead::setFile(const std::string &path, const std::string &mode) {
    closeFile();
    mode_=mode;
    if(mode=="r")
        file_in_.open(path,std::ios::in);
    else if(mode=="w")
        file_out_.open(path,std::ios::out);
    else
        mode_="";
}

void InfoRead::closeFile() {
    if(mode_=="r")
        file_in_.close();
    if(mode_=="w")
        file_out_.close();
    mode_="";
}

Info InfoRead::getInfo() {
    if(empty()||mode_=="w")
        return Info();
    else
    {
        char s[101]={0};
        Info res;
        while(!file_in_.eof()&&file_in_.getline(s,100))
        {
            if(s[0]!='#'&&s[0]!=0)
            {
                res=InfoRead::understandInfo(s);
                if(!res.empty())
                {
                    return res;
                }
            }
        }
    }
    return Info();
}

std::vector<Info> InfoRead::getInfos() {
    if(empty()||mode_=="w")
        return std::vector<Info>();
    else{
        std::vector<Info> result;
        char s[101]={0};
        Info res;
        while(!file_in_.eof()&&file_in_.getline(s,100))
        {
            if(s[0]!='#'&&s[0]!=0)
            {
                res=InfoRead::understandInfo(s);
                if(!res.empty())
                {
                    result.emplace_back(res);
                }
            }
        }
        return result;
    }
}

void InfoRead::writeInfo(const Info &s) {
    if(mode_=="r"||empty())
        return;
    else
    {
        file_out_ << s << std::endl;
    }
}

void InfoRead::writeInfos(const std::vector<Info> &s) {
    if(mode_=="r"||empty())
        return;
    else
    {
        for(auto& i:s)
            file_out_<<i<<std::endl;
    }

}

Info InfoRead::understandInfo(const std::string &s) {
    Info res;
    bool head=false;
    bool name=false;
    bool end=false;
    bool mode=false;
    bool value=false;
    std::string str="";
    bool end_of_str=true;
    if(s[0]=='#')
        return Info();
    for(auto i:s)
    {
        if(end==true)
            break;
        if(i=='<')
        {
            head=true;
        }
        else if(i=='|')
        {
            if(!head)
            {
                return Info();
            }
            else
            {
                end_of_str=true;
            }
        }
        else if(i==' ')
        {
            end_of_str=true;
        }
        else if(i=='>')
        {
            end_of_str=true;
        }
        else {
            end_of_str = false;
            str+=i;
        }
        if(end_of_str&&!str.empty())
        {
            if(!name)
            {
                name=true;
                res.setName(str);
            }
            else if(!mode)
            {
                mode=true;
            }
            else
            {
                value=true;
                res.addValue(str);
            }
            end_of_str=true;
            str="";
        }
    }
    if(!name||!mode||!value)
        return Info();
    return res;
}
