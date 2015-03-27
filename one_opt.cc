/**
 * @file one_opt.cc
 * @brief Definitions for OneOpt class.
 * @author Nicu Tofan <nicu.tofan@gmail.com>
 * @copyright Copyright 2014 piles contributors. All rights reserved.
 * This file is released under the
 * [MIT License](http://opensource.org/licenses/mit-license.html)
 */

#include "appopts.h"
#include "appopts-private.h"
#include "one_opt.h"

#include <usermsg/usermsg.h>
#include <usermsg/usermsgman.h>
#include <perst/perst_factory.h>
#include <perst/perst.h>

#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QFile>

/**
 * @class OneOpt
 *
 * This class describes an option and contains a default value for it. The class
 * does NOT hold the value for the variable.
 * Lists of such instances are used to initialize the AppOpts class by only reading
 * the variables we know about.
 */

/* ------------------------------------------------------------------------- */
/**
 *
 * @param name The name of the option.
 * @param stgs_group The name of the group where the option lies in settings file.
 * @param description Human readable description.
 * @param default_val Default value.
 * @return Newly initialized object.
 */
OneOpt OneOpt::create (
        const QString name, const QString stgs_group,
        const QString description, const QStringList default_val)
{
    OneOpt result;

    result.name_ = name;
    result.group_ = stgs_group;
    result.description_ = description;
    result.default_ = default_val;

    return result;
}
/* ========================================================================= */
