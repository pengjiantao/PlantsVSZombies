//
// Created by fiyqkrc on 12/27/20.
//

#include "Info.h"

infoStyle Info::Style() const {
    return this->style_;
}

Info::Info(infoStyle s) {
    style_=s;
    empty_=true;
    value_="";
    name_="";
}

Info::Info() {
    style_=infoStyle::single;
    empty_=true;
    value_="";
    name_="";
}

bool Info::empty() const {
    return empty_;
}

std::string Info::Value() const {
    return value_;
}

void Info::setValue(const std::string &s) {
    if(style_==infoStyle::list) {
        style_=infoStyle::single;
        value_list_.clear();
    }
    value_=s;
}

std::vector<std::string> Info::ValueList() const {
    return value_list_;
}

void Info::clear() {
    style_=infoStyle::single;
    empty_=true;
    value_list_.clear();
    value_="";
    name_="";
}

Info::Info(Info &&src) noexcept {
    this->value_=move(src.value_);
    this->value_list_=move(src.value_list_);
    this->style_=src.style_;
    this->empty_=src.empty_;
    this->name_=move(src.name_);
}

Info &Info::operator=(Info &&src) noexcept {
    if(this==&src)
    {
        return *this;
    }
    else
    {
        this->value_=move(src.value_);
        this->value_list_=move(src.value_list_);
        this->style_=src.style_;
        this->empty_=src.empty_;
        this->name_=src.name_;
        return *this;
    }
}

Info::Info(const Info &src) {
    this->value_=src.value_;
    this->value_list_=src.value_list_;
    this->style_=src.style_;
    this->empty_=src.empty_;
    this->name_=src.name_;
}

Info &Info::operator=(const Info &src) {
    if(this==&src)
    {
        return *this;
    }
    else
    {
        this->value_=src.value_;
        this->value_list_=src.value_list_;
        this->style_=src.style_;
        this->empty_=src.empty_;
        this->name_=src.name_;
        return *this;
    }
}

void Info::addValue(const std::string &s) {
    if(empty_ && style_ == infoStyle::single) {
        value_ = s;
        empty_= false;
    }
    else if(empty_&&style_==infoStyle::list)
    {
        this->value_list_.emplace_back(s);
        empty_= false;
    }
    else
    {
        if(style_==infoStyle::single) {

            style_ = infoStyle::list;
            value_list_.emplace_back(value_);
            value_="";
        }
        this->value_list_.emplace_back(s);
    }
}

void Info::setName(const std::string& s) {
    name_=s;
}

std::string Info::Name() const {
    return name_;
}


Info::~Info() = default;

std::ostream& operator<<(std::ostream& ostream,const Info& s)
{
    ostream<<"< "<<s.Name();
    if(s.Style()==infoStyle::single)
        ostream<<" / "<<"single"<<" / "<<s.Value()<<" >";
    else {
        ostream << " / " << "list"<<" /";
        for(auto& i :s.ValueList())
        {
            ostream<<" "<<i.data();
        }
        ostream<<" >";
    }
    return ostream;
}
