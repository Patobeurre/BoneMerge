#ifndef MERGEEXCEPTION_H
#define MERGEEXCEPTION_H

#include <exception>
#include <string>


class MergeException : public std::exception
{
public:
    explicit MergeException(const char* message): msg_(message) { }
    explicit MergeException(const std::string& message): msg_(message) {}
    MergeException(const std::string& title,
                   const std::string& message,
                   const std::string& details): msg_(message), title_(title), details_(details) {}
    virtual ~MergeException() throw () {}
    virtual const char* what() const throw () { return msg_.c_str(); }
    std::string getTitle() { return title_; }
    std::string getMessage() { return msg_; }
    std::string getDetails() { return details_; }
protected:
    std::string title_;
    std::string details_;
    std::string msg_;
};

#endif // MERGEEXCEPTION_H
