/**
 * @file one_opt_list.h
 * @brief Declarations for OneOptList class
 * @author Nicu Tofan <nicu.tofan@gmail.com>
 * @copyright Copyright 2014 piles contributors. All rights reserved.
 * This file is released under the
 * [MIT License](http://opensource.org/licenses/mit-license.html)
 */

#ifndef GUARD_APPOPTS_ONEOPTLIST_H_INCLUDE
#define GUARD_APPOPTS_ONEOPTLIST_H_INCLUDE

#include <appopts/appopts-config.h>
#include <appopts/one_opt.h>

#include <QMap>
#include <QString>
#include <QStringList>

class UserMsg;
class PerSt;

//! A list of option definitions.
class APPOPTS_EXPORT OneOptList : public QList<OneOpt> {

public:

    void
    append (
            const QString name,
            const QString stgs_group = "general",
            const QString description = QString(),
            const QStringList default_val = QStringList());

protected:


private:


};

#endif // GUARD_APPOPTS_ONEOPTLIST_H_INCLUDE
