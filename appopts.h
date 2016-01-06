/**
 * @file appopts.h
 * @brief Declarations for AppOpts class
 * @author Nicu Tofan <nicu.tofan@gmail.com>
 * @copyright Copyright 2014 piles contributors. All rights reserved.
 * This file is released under the
 * [MIT License](http://opensource.org/licenses/mit-license.html)
 */

#ifndef GUARD_APPOPTS_H_INCLUDE
#define GUARD_APPOPTS_H_INCLUDE

#include <appopts/appopts-config.h>

#include <QMap>
#include <QString>
#include <QStringList>

class UserMsg;
class PerSt;
class OneOpt;
class OneOptList;

//! Application options.
class APPOPTS_EXPORT AppOpts : public QMap<QString,QStringList> {


private:

    //! copy constructor
    AppOpts (const AppOpts & other) :
        system_file_(other.system_file_),
        user_file_(other.user_file_),
        local_file_(other.local_file_),
        current_file_(other.current_file_)
    {}

    //! assignment operator
    AppOpts& operator=( const AppOpts& other) {
        system_file_ = other.system_file_;
        user_file_ = other.user_file_;
        local_file_ = other.local_file_;
        current_file_ = other.current_file_;
        return *this;
    }

public:

    //! Default constructor.
    AppOpts ();

    //! Destructor.
    virtual ~AppOpts();

    //! Load options from files.
    bool
    loadFromAll (
            UserMsg & um,
            const QString & s_app_name = QString());

    //! Load options from a file.
    bool
    loadFile (
            const QString & s_file,
            PerSt ** out_pers,
            UserMsg & um);

    //! Looks into existing files for requested variable.
    bool
    readValueFromCfgs (
            const OneOpt & opt,
            UserMsg & um);

    //! Looks into existing files for requested variables.
    bool
    readMultipleFromCfgs (
            const OneOptList & optlist,
            UserMsg & um);

    //! Set a value.
    void
    setValue (
            const QString & s_key,
            const QString & s_value);

    //! Set a value.
    void
    setValue (
            const QString & s_key,
            const QStringList & sl_value);

    //! Append a value.
    void
    appendValue (
            const QString & s_key,
            const QString & s_value);

    //! Append values.
    void
    appendValues (
            const QString & s_key,
            const QStringList & sl_values);

    //! Set current file.
    bool
    setCurrentConfig (
            const QString & s_file,
            UserMsg & um);

    //! Get a Boolean value based on option's name.
    bool
    valueB (
            const QString & s_name,
            bool b_default = false) const;

    //! Get a string value.
    QString
    valueS (
            const QString & s_name,
            const QString & s_default = QString()) const;

    //! Get a list of strings.
    QStringList
    valueSL (
            const QString & s_name,
            const QStringList & sl_default = QStringList()) const;

    //! Get an integer value.
    int
    valueI (
            const QString & s_name,
            int i_default = 0) const;

    //! Get an integer value.
    double
    valueD (
            const QString & s_name,
            double d_default = 0.0) const;

public:

    //! Get a proper name starting from a template.
    static QString
    cfgFileName (
            const QString &s_app_name);

protected:


private:

    //! Uses one Perst instance to find requested option.
    bool
    readValueFromPerSt (
            PerSt *perst,
            const OneOpt & opt,
            UserMsg & um);

    PerSt * system_file_; /**< configuration file at system level */
    PerSt * user_file_; /**< configuration file at user level */
    PerSt * local_file_; /**< configuration file at local level */
    PerSt * current_file_; /**< current used for saving things */

public: virtual void anchorVtable() const;
};


#endif // GUARD_APPOPTS_H_INCLUDE
