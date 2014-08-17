#ifndef HARBOUR_CALLRECORDERD_CALLRECORDEREXCEPTION_H
#define HARBOUR_CALLRECORDERD_CALLRECORDEREXCEPTION_H

#include <exception>

#include <QString>

class CallRecorderException : public std::exception
{
public:
    CallRecorderException(const QString& what): mWhat(what) { }
    ~CallRecorderException() throw()
    {
    }

    const char* what() const throw() { return mWhat.toUtf8().data(); }
    QString qWhat() { return mWhat; }

private:
    QString mWhat;
};

#endif // CALLRECORDEREXCEPTION_H
