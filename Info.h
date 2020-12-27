//
// Created by fiyqkrc on 12/27/20.
//

#ifndef INFOREAD_INFO_H
#define INFOREAD_INFO_H
#include<string>
#include<iostream>
#include<vector>
enum class infoStyle{
    single,list
};

class Info {
private:
    infoStyle style_;
    bool empty_;
    std::string value_;
    std::vector<std::string> value_list_;
    std::string name_;
public:
    std::string Name() const;
    void setName( const std::string& s);
    infoStyle Style() const;
    bool empty() const;
    std::string Value() const;
    std::vector<std::string> ValueList() const;
    void clear();
    void setValue(const std::string& s);
    void addValue(const std::string& s);
    Info();
    Info(Info&& src) noexcept;
    Info& operator=(Info&& src) noexcept;
    Info(const Info& src);
    Info& operator=(const Info& src);
    explicit Info(infoStyle);
    ~Info();
};

std::ostream& operator<<(std::ostream& ostream,const Info& s);

#endif //INFOREAD_INFO_H
