/**
 * @file one_opt_list.cc
 * @brief Definitions for OneOptList class.
 * @author Nicu Tofan <nicu.tofan@gmail.com>
 * @copyright Copyright 2014 piles contributors. All rights reserved.
 * This file is released under the
 * [MIT License](http://opensource.org/licenses/mit-license.html)
 */

#include "appopts.h"
#include "appopts-private.h"
#include "one_opt.h"
#include "one_opt_list.h"

#include <usermsg/usermsg.h>
#include <usermsg/usermsgman.h>
#include <perst/perst_factory.h>
#include <perst/perst.h>

#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QFile>

/**
 * @class OneOptList
 *
 *
 */


void OneOptList::append (
        const QString name, const QString stgs_group,
        const QString description, const QStringList default_val)
{
    this->QList<OneOpt>::append (OneOpt::create (name, stgs_group, description, default_val));
}
