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
 * This class holds a list of key-value pairs, where values are ultimatelly
 * list of strings.
 */

#define CFG_GROUP_GENERAL "General"
#define CFG_PERST_VERSION "perst_version"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#define QT_DATA_LOC QStandardPaths::AppDataLocation
#else
#define QT_DATA_LOC QStandardPaths::DataLocation
#endif


/* ------------------------------------------------------------------------- */
/**
 * Detailed description for constructor.
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
 * Detailed description for destructor.
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
 *
 * The name that results is then used to search for a configuration file
 * in three locations: system data directory, user home directory and
 * current directory. If found, the file is immediatly loaded in this order.
 *
 * The reverse order (current dir, user home, system data) is used to decide
 * where to save changed settings.
 *
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
 * Creates a PerSt instance from our file and checks that a `General`
 * section exists and it contains a proper `` version string that we are safe
 * to interpret.
 *
 * @param s_file Input file's path.
 * @param out_pers Resulted PerSt object, if any.
 * @param um Communication device.
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

        b_ret = user_file->endGroup   (CFG_GROUP_GENERAL);
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
bool AppOpts::readValueFromPerSt (
        PerSt * perst, const OneOpt & opt, UserMsg & um)
{
    bool b_ret = false;
    APPOPTS_TRACE_ENTRY;

    if (perst != NULL) {
        if (opt.group_.isEmpty()) {
            perst->beginGroup (opt.group_);
        }

        if (perst->hasKey (opt.name_)) {
            QStringList sl = perst->valueSList (opt.name_);
            insert (opt.name_, sl);
            b_ret = true;
        }

        if (opt.group_.isEmpty()) {
            perst->endGroup (opt.group_);
        }
    }

    APPOPTS_TRACE_EXIT;
    return b_ret;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
bool AppOpts::readValueFromCfgs (const OneOpt & opt, UserMsg & um)
{
    APPOPTS_TRACE_ENTRY;

    bool b_ret = true;
    for (;;) {

        b_ret = b_ret & readValueFromPerSt (system_file_, opt, um);
        b_ret = b_ret & readValueFromPerSt (user_file_, opt, um);
        b_ret = b_ret & readValueFromPerSt (local_file_, opt, um);

        if (!b_ret && opt.required_) {
            um.addErr (QString (
                           "Required option %1 not present in "
                           "configuration file(s).")
                       .arg (opt.name_));
            b_ret = false;
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
void AppOpts::setValue (
        const QString & s_key, const QString & s_value)
{
    insert (s_key, QStringList(s_value));
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void AppOpts::setValue (
        const QString & s_key, const QStringList & sl_value)
{
    insert (s_key, sl_value);
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void AppOpts::appendValue (
        const QString & s_key, const QString & s_value)
{
    QMap<QString,QStringList>::iterator found = find (s_key);
    QMap<QString,QStringList>::iterator endi = end();
    if (found == endi) {
        insert (s_key, QStringList(s_value));
    } else {
        //QStringList s_found = ;
        found.value().append (s_value);
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void AppOpts::appendValues (
        const QString & s_key, const QStringList & sl_values)
{
    QMap<QString,QStringList>::iterator found = find (s_key);
    QMap<QString,QStringList>::iterator endi = end();
    if (found == endi) {
        insert (s_key, sl_values);
    } else {
        //QStringList s_found = ;
        found.value().append (sl_values);
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
bool AppOpts::setCurrentConfig (const QString & s_file, UserMsg & um)
{
    bool b_ret = false;
    PerSt * new_current_ = NULL;

    for (;;) {
        if (s_file == "system") {
            if (system_file_ == NULL) {
                um.addErr (QObject::tr(
                               "System config file was not found; "
                               "it can't be made current."));
                break;
            }
            new_current_ = system_file_;
        } else if (s_file == "user") {
            if (user_file_ == NULL) {
                um.addErr (QObject::tr(
                               "User config file was not found; "
                               "it can't be made current."));
                break;
            }
            new_current_ = user_file_;
        } else if (s_file == "local") {
            if (local_file_ == NULL) {
                um.addErr (QObject::tr(
                               "Local config file was not found; "
                               "it can't be made current."));
                break;
            }
            new_current_ = local_file_;
        } else if (s_file.isEmpty ()) {
            um.addErr (QObject::tr(
                           "No file config provided."));
            break;
        } else {
            if (system_file_ != NULL) {
                if (s_file == system_file_->location()) {
                    s_file == "system";
                    continue;
                }
            } else if (user_file_ != NULL) {
                if (s_file == user_file_->location()) {
                    s_file == "user";
                    continue;
                }
            } else if (local_file_ != NULL) {
                if (s_file == local_file_->location()) {
                    s_file == "local";
                    continue;
                }
            } else if (!QFile::exists (s_file)) {
                um.addErr (QObject::tr(
                               "Config file does not exist; "
                               "it can't be made current."));
                break;
            } else if (!loadFile (s_file, &new_current_, um)) {
                um.addErr (QObject::tr(
                               "Config file contains errors and was not loaded; "
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
            return
                    (sl_result.at (0) == "TRUE") ||
                    (sl_result.at (0) == "true") ||
                    (sl_result.at (0) == "1");
        }
    }
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
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
