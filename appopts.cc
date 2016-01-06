/**
 * @file appopts.cc
 * @brief Definitions for AppOpts class.
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
 * @class AppOpts
 *
 * This class holds a list of key-value pairs, where values are ultimately
 * list of strings.
 */

#define CFG_GROUP_GENERAL "general"
#define CFG_PERST_VERSION "perst_version"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#define QT_DATA_LOC QStandardPaths::AppDataLocation
#else
#define QT_DATA_LOC QStandardPaths::DataLocation
#endif


/* ------------------------------------------------------------------------- */
/**
 * Creates a valid instance.
 */
AppOpts::AppOpts() : QMap<QString,QStringList>(),
    system_file_(NULL),
    user_file_(NULL),
    local_file_(NULL),
    current_file_(NULL)
{
    APPOPTS_TRACE_ENTRY;

    APPOPTS_TRACE_EXIT;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * Releases all resources associated with this instance.
 */
AppOpts::~AppOpts()
{
    APPOPTS_TRACE_ENTRY;
    if (current_file_ != NULL) {
        if (
                (current_file_ != system_file_) &&
                (current_file_ != user_file_) &&
                (current_file_ != local_file_)) {
            delete current_file_;
            current_file_ = NULL;
        }
    }

    if (system_file_ != NULL) {
        delete system_file_;
        system_file_ = NULL;
    }
    if (user_file_ != NULL) {
        delete user_file_;
        user_file_ = NULL;
    }
    if (local_file_ != NULL) {
        delete local_file_;
        local_file_ = NULL;
    }
    APPOPTS_TRACE_EXIT;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The method will replace spaces by `_` and will make all letters lower-case.
 * If input template is empty then the result is `config`.
 *
 * @param s_app_name Input template.
 * @return the name of the config file without extension.
 */
QString AppOpts::cfgFileName (const QString & s_app_name)
{
    if (s_app_name.isEmpty()) {
        return "config";
    } else {
        QString result = s_app_name;
        result.replace(QChar(' '), QChar('_'));
        return result.toLower();
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The function will either get the name of the application or use provided
 * string. The spaces are replaced by `_` and all letters are lower-cased.
 * See `cfgFileName()` for details.
 *
 * The name that results is then used to search for a configuration file
 * in three locations: system data directory, user home directory and
 * current directory. If found, the file is immediately loaded in this order.
 *
 * The reverse order (current dir, user home, system data) is used to decide
 * where to save changed settings.
 *
 * @param um structure used to show messages.
 * @param s_app_name the name to use for file name.
 * @return true if everything went fine.
 */
bool AppOpts::loadFromAll (UserMsg & um, const QString & s_app_name)
{
    bool b_ret = true;
    for (;;) {

        //! Get the name of the config file
        QString s_file_name;
        if (s_app_name.isEmpty()) {
            s_file_name = cfgFileName (QCoreApplication::applicationName());
        } else {
            s_file_name = cfgFileName (s_app_name);
        }
        s_file_name = QString("%1.ini").arg (s_file_name);
        um.addDbgInfo (QString("Looking for a config file named %1.")
                       .arg (s_file_name));

        // look for a config file in data location
        QString s_file_system = QStandardPaths::locate (
                    QT_DATA_LOC,
                    s_file_name,
                    QStandardPaths::LocateFile);
        if (!s_file_system.isEmpty ()) {
            bool b_tmp = loadFile (s_file_system, &system_file_, um);
            um.addDbgInfo (QString("Located general config file %1; load result: %2.")
                           .arg (s_file_system)
                           .arg (b_tmp ? "loaded" : "failed"));
            b_ret = b_ret & b_tmp;
        }

        // look for a config file in home location
        QString s_file_user = QStandardPaths::locate (
                    QStandardPaths::HomeLocation,
                    s_file_name,
                    QStandardPaths::LocateFile);
        if (!s_file_user.isEmpty () && (s_file_user != s_file_system)) {
            bool b_tmp = loadFile (s_file_user, &system_file_, um);
            um.addDbgInfo (QString("Located user config file %1; load result: %2.")
                           .arg (s_file_system)
                           .arg (b_tmp ? "loaded" : "failed"));
            b_ret = b_ret & b_tmp;
        }

        // read local configuration
        QDir d_crt (QDir::current ());
        QString s_file_local = d_crt.absoluteFilePath (s_file_name);
        if (QFile::exists (s_file_local)) {
            if ((s_file_local != s_file_system) &&
                    (s_file_local != s_file_user)) {

                bool b_tmp = loadFile (s_file_local, &local_file_, um);

                um.addDbgInfo (QString("Current dir config file %1; load result: %2.")
                               .arg (s_file_system)
                               .arg (b_tmp ? "loaded" : "failed"));
                b_ret = b_ret & b_tmp;
            }
        }

        // select where we will save changes
        QString s_save;
        if (local_file_ != NULL) {
            current_file_ = local_file_;
            s_save = "current dir";
        } else if (user_file_ != NULL) {
            current_file_ = user_file_;
            s_save = "user home";
        } else if (system_file_ != NULL) {
            current_file_ = system_file_;
            s_save = "system data";
        }
        if (s_save.isEmpty()) {
            um.addDbgInfo (QString ("Changes will NOT be saved because no config file was found"));
        } else {
            um.addDbgInfo (QString ("Changes will be saved in %1 file: %2")
                           .arg (s_save)
                           .arg (current_file_->location()));
        }

        break;
    }
    return b_ret;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * Creates a PerSt instance from our file and checks that a `general`
 * section exists and it contains a proper `perst_version` version
 * string that we are safe to interpret.
 *
 * @param s_file Input file's path.
 * @param out_pers Resulted PerSt object, if any.
 * @param um communication device.
 * @return true if everything went fine.
 */
bool AppOpts::loadFile (const QString & s_file, PerSt ** out_pers,
                        UserMsg & um)
{
    PerSt * user_file = NULL;
    bool b_ret = false;
    for (;;) {

        // parse the file
        user_file = PerStFactory::create ("config", s_file);
        if (user_file == NULL) break;

        b_ret = user_file->beginGroup (CFG_GROUP_GENERAL);
        if (!b_ret) break;

        // read the version
        QString s_version = user_file->valueS (CFG_PERST_VERSION);
        if (!s_version.isEmpty ()) {
            this->insert (CFG_PERST_VERSION, QStringList (s_version));
        }

        // the only valid version right now is ours
        if (s_version != APPOPTS_VERSION_STRING) {
            um.addErr (
                       QString("The version of the file (%1) differs from supported version (%2).")
                       .arg (s_version)
                       .arg (APPOPTS_VERSION_STRING));
        }

        b_ret = user_file->endGroup (CFG_GROUP_GENERAL);
        if (!b_ret) break;

        b_ret = true;
        break;
    }

    if (!b_ret) {
        delete user_file;
        user_file = NULL;
    }

    if (out_pers != NULL) {
        *out_pers = user_file;
    }

    return b_ret;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The method will honor variable's group and try to locate the value
 * in given persistent storage.
 * If found the variable and the value are inserted into local storage,
 * replacing any value that was previously there. The name of the option
 * will be a combination of the name of the group and the name of the variable:
 *
 *     group/variable
 *
 * @note Default value is not used if the variable is not found.
 *
 * @param perst the object to search (can be NULL)
 * @param opt   definition of the variable to search
 * @param um    communication object
 * @return true if the variable was found
 */
bool AppOpts::readValueFromPerSt (
        PerSt * perst, const OneOpt & opt, UserMsg & um)
{
    bool b_ret = false;
    APPOPTS_TRACE_ENTRY;

    if (perst != NULL) {
        if (!opt.group_.isEmpty()) {
            perst->beginGroup (opt.group_);
        }

        if (perst->hasKey (opt.name_)) {
            QStringList sl = perst->valueSList (opt.name_);
            insert (opt.fullName(), sl);
            um.addDbgInfo( (QString (
                                "Option %1 found in "
                                "configuration file %2.")
                            .arg(opt.name_)
                            .arg(perst->location())));
            b_ret = true;
        }

        if (!opt.group_.isEmpty()) {
            perst->endGroup (opt.group_);
        }
    }

    APPOPTS_TRACE_EXIT;
    return b_ret;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The method will honor variable's group and try to locate the value
 * in system, user and local config files (represented as persistent storage
 * inside).
 * If found the variable and the value are inserted into local storage,
 * replacing any value that was previously there.
 *
 * If a variable is not required and is also not found in the configuration
 * files the default value is used.
 *
 * @param opt   definition of the variable to search
 * @param um    communication object
 * @return true if the variable was found in at least one file
 */
bool AppOpts::readValueFromCfgs (const OneOpt & opt, UserMsg & um)
{
    APPOPTS_TRACE_ENTRY;

    bool b_ret = false;
    for (;;) {

        b_ret = b_ret | readValueFromPerSt (system_file_, opt, um);
        b_ret = b_ret | readValueFromPerSt (user_file_, opt, um);
        b_ret = b_ret | readValueFromPerSt (local_file_, opt, um);

        if (!b_ret) {
            if (opt.required_) {
                um.addErr (QString (
                               "Required option %1 not present in "
                               "configuration file(s).")
                           .arg (opt.name_));
                b_ret = false;
            } else {
                QStringList sl = opt.default_;
                insert (opt.fullName(), sl);
            }
        } else {
            b_ret = true;
        }

        break;
    }

    APPOPTS_TRACE_EXIT;
    return b_ret;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The method simply iterates the list of options and uses
 * `readValueFromCfgs()` to get their values and insert them into its own
 * option tree.
 *
 * This is useful if the application defines at startup a list of options
 * that it expects, then initializes the AppOpts instance and makes sure
 * that all required options are present.
 *
 * @param list the list of variables to search
 * @param um communication object
 * @return true if all required variables were found
 */
bool AppOpts::readMultipleFromCfgs (const OneOptList & list, UserMsg & um)
{
    APPOPTS_TRACE_ENTRY;

    bool b_ret = true;

    foreach (const OneOpt & opt, list) {
        b_ret = b_ret & readValueFromCfgs (opt, um);
    }

    APPOPTS_TRACE_EXIT;
    return b_ret;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings. This
 * overload will create a list with a single member and use that as a value.
 *
 * Any value that was previously assigned to this option will be overwritten.
 *
 * @param s_key the name of the variable to change
 * @param s_value a string value to use
 */
void AppOpts::setValue (
        const QString & s_key, const QString & s_value)
{
    insert (s_key, QStringList(s_value));
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings.
 * The \b sl_value argument is directly used in internal map.
 *
 * Any value that was previously assigned to this option will be overwritten.
 *
 * @param s_key the name of the variable to change
 * @param s_value a string value to use
 */
void AppOpts::setValue (
        const QString & s_key, const QStringList & sl_value)
{
    insert (s_key, sl_value);
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings. This
 * overload will either create a list with a single member and use that as
 * a value if the option is not found or append \b s_value to internal list
 * associated with this option.
 *
 * @param s_key the name of the variable to change
 * @param s_value a string value to use
 */
void AppOpts::appendValue (
        const QString & s_key, const QString & s_value)
{
    QMap<QString,QStringList>::iterator found = find (s_key);
    QMap<QString,QStringList>::iterator endi = end();
    if (found == endi) {
        insert (s_key, QStringList(s_value));
    } else {
        found.value().append (s_value);
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings. If the option
 * is already present inside t6he two lists will be merged.
 *
 * @param s_key the name of the variable to change
 * @param s_value a string value to use
 */
void AppOpts::appendValues (
        const QString & s_key, const QStringList & sl_values)
{
    QMap<QString,QStringList>::iterator found = find (s_key);
    QMap<QString,QStringList>::iterator endi = end();
    if (found == endi) {
        insert (s_key, sl_values);
    } else {
        found.value().append (sl_values);
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * Current config file is meaningful if the values (maybe adjusted by the
 * application) need to be saved back to a file.
 *
 * The \b s_file may be one of the following keywords:
 * - system
 * - user
 * - local
 * In this case, if the coresponding persistent object is valid, the
 * file is used as current file.
 *
 * If \b s_file is an actual file that is different from the files
 * used as system, user and local files the file is loaded and
 * made current.
 *
 * A previous current file that is not one of the three types
 * (loaded by a previous call to `setCurrentConfig()`)
 * will be discarded by this method.
 *
 * @param s_file keyword or the path and name of the file to make current
 * @param um communication object
 * @return true if everything went well
 */
bool AppOpts::setCurrentConfig (const QString & s_file_input, UserMsg & um)
{
    bool b_ret = false;
    PerSt * new_current_ = NULL;
    QString s_file = s_file_input;
    for (;;) {
        if (s_file == "system") {
            if (system_file_ == NULL) {
                um.addErr (QObject::tr(
                               "System configuration file was not found; "
                               "it can't be made current."));
                break;
            }
            new_current_ = system_file_;
        } else if (s_file == "user") {
            if (user_file_ == NULL) {
                um.addErr (QObject::tr(
                               "User configuration file was not found; "
                               "it can't be made current."));
                break;
            }
            new_current_ = user_file_;
        } else if (s_file == "local") {
            if (local_file_ == NULL) {
                um.addErr (QObject::tr(
                               "Local configuration file was not found; "
                               "it can't be made current."));
                break;
            }
            new_current_ = local_file_;
        } else if (s_file.isEmpty ()) {
            um.addErr (QObject::tr(
                           "No configuration file was provided."));
            break;
        } else {
            if (system_file_ != NULL) {
                if (s_file == system_file_->location()) {
                    s_file = "system";
                    continue;
                }
            } else if (user_file_ != NULL) {
                if (s_file == user_file_->location()) {
                    s_file = "user";
                    continue;
                }
            } else if (local_file_ != NULL) {
                if (s_file == local_file_->location()) {
                    s_file = "local";
                    continue;
                }
            } else if (!QFile::exists (s_file)) {
                um.addErr (QObject::tr(
                               "Configuration file does not exist; "
                               "it can't be made current."));
                break;
            } else if (!loadFile (s_file, &new_current_, um)) {
                um.addErr (QObject::tr(
                               "Configuration file contains errors and was not loaded; "
                               "it can't be made current."));
                break;
            }
        }
        b_ret = true;
        break;
    }
    if (b_ret) {
        if (current_file_ != NULL) {
            if ((current_file_ != system_file_) &&
                    (current_file_ != user_file_) &&
                    (current_file_ != local_file_)) {
                delete current_file_;
                current_file_ = NULL;
            }
        }
        current_file_ = new_current_;
    }
    return b_ret;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings. For boolean
 * values first entry in the list is used and the string is compared against
 * `FALSE`, `false` and `0`; if the string matches the value is false,
 * otherwise it is true.
 *
 * @param s_name name of the option to retrieve
 * @param b_default default value if the option is not found
 * @return the result
 */
bool AppOpts::valueB (
        const QString & s_name, bool b_default) const
{
    QMap<QString,QStringList>::const_iterator found = find (s_name);
    QMap<QString,QStringList>::const_iterator endi = end();
    if (found == endi) {
        return b_default;
    } else {
        QStringList sl_result = *found;
        if (sl_result.count () == 0) {
            return b_default;
        } else {
            return !(
                    (sl_result.at (0) == "FALSE") ||
                    (sl_result.at (0) == "false") ||
                    (sl_result.at (0) == "0"));
        }
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings. For integer
 * values first entry in the list is used and converted into integer.
 * If the conversion fails the default value is returned.
 *
 * @param s_name name of the option to retrieve
 * @param i_default default value if the option is not found or
 *        can't be converted
 * @return the integer
 */
int AppOpts::valueI (
        const QString & s_name, int i_default) const
{
    QMap<QString,QStringList>::const_iterator found = find (s_name);
    QMap<QString,QStringList>::const_iterator endi = end();
    if (found == endi) {
        return i_default;
    } else {
        QStringList sl_result = *found;
        if (sl_result.count () == 0) {
            return i_default;
        } else {
            bool b_ok = false;
            int result = sl_result.at (0).toInt (&b_ok);
            if (!b_ok) {
                return i_default;
            } else {
                return result;
            }
        }
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings. For double
 * values first entry in the list is used and converted into double.
 * If the conversion fails the default value is returned.
 *
 * @param s_name name of the option to retrieve
 * @param d_default default value if the option is not found or
 *        can't be converted
 * @return the number
 */
double AppOpts::valueD (
        const QString & s_name, double d_default) const
{
    QMap<QString,QStringList>::const_iterator found = find (s_name);
    QMap<QString,QStringList>::const_iterator endi = end();
    if (found == endi) {
        return d_default;
    } else {
        QStringList sl_result = *found;
        if (sl_result.count () == 0) {
            return d_default;
        } else {
            bool b_ok = false;
            double result = sl_result.at (0).toDouble (&b_ok);
            if (!b_ok) {
                return d_default;
            } else {
                return result;
            }
        }
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings. For string
 * values first entry in the list is used.
 *
 * @param s_name name of the option to retrieve
 * @param s_default default value if the option is not found
 * @return the string that was found
 */
QString AppOpts::valueS (
        const QString & s_name, const QString & s_default) const
{
    QMap<QString,QStringList>::const_iterator found = find (s_name);
    QMap<QString,QStringList>::const_iterator endi = end();
    if (found == endi) {
        return s_default;
    } else {
        QStringList sl_result = *found;
        if (sl_result.count () == 0) {
            return s_default;
        } else {
            return sl_result.at (0);
        }
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/**
 * The class represents values for options as a list of strings.
 * Entire list is returned if found.
 *
 * An empty list is interpreted as a missing value and default list is returned.
 *
 * @param s_name name of the option to retrieve
 * @param s_default default value if the option is not found
 * @return the list that was found
 */
QStringList AppOpts::valueSL (
        const QString & s_name, const QStringList & sl_default) const
{
    QMap<QString,QStringList>::const_iterator found = find (s_name);
    QMap<QString,QStringList>::const_iterator endi = end();
    if (found == endi) {
        return sl_default;
    } else {
        QStringList sl_result = *found;
        if (sl_result.count () == 0) {
            return sl_default;
        } else {
            return sl_result;
        }
    }
}
/* ========================================================================= */

void AppOpts::anchorVtable() const {}
