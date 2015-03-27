/**
 * @file one_opt.h
 * @brief Declarations for OneOpt class
 * @author Nicu Tofan <nicu.tofan@gmail.com>
 * @copyright Copyright 2014 piles contributors. All rights reserved.
 * This file is released under the
 * [MIT License](http://opensource.org/licenses/mit-license.html)
 */

#ifndef GUARD_APPOPTS_ONEOPT_H_INCLUDE
#define GUARD_APPOPTS_ONEOPT_H_INCLUDE

#include <appopts/appopts-config.h>

#include <QMap>
#include <QString>
#include <QStringList>

class UserMsg;
class PerSt;

//! Definition for an option.
class APPOPTS_EXPORT OneOpt {

public:

    QString name_;
    QString group_;
    QString description_;
    QStringList default_;
    bool required_;

    //! Populates an instance.
    static OneOpt
    create (
            const QString name,
            const QString stgs_group = "General",
            const QString description = QString(),
            const QStringList default_val = QStringList());


    //! Default constructor.
    OneOpt() :
        name_(),
        group_(),
        description_(),
        default_(),
        required_(false)
    {}


protected:


private:


};

#endif // GUARD_APPOPTS_ONEOPT_H_INCLUDE
