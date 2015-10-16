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
            const QString stgs_group = "general",
            const QString description = QString(),
            const QStringList default_val = QStringList());


    //! Default constructor.
    explicit OneOpt() :
        name_(),
        group_(),
        description_(),
        default_(),
        required_(false)
    {}

    //! copy constructor
    ///
    OneOpt (const OneOpt & other) :
        name_(other.name_),
        group_(other.group_),
        description_(other.description_),
        default_(other.default_),
        required_(other.required_)
    {}

    //! assignment operator
    ///
    OneOpt& operator=( const OneOpt& other ) {
        name_ = other.name_;
        group_ = other.group_;
        description_ = other.description_;
        default_ = other.default_;
        required_ = other.required_;
        return *this;
    }

    //! The name.
    ///
    inline const QString &
    name () const {
        return name_;
    }

    //! Change the name.
    ///
    inline void
    setName (const QString & value) {
        name_ = value;
    }

    //! Full name (includes the group)
    inline QString fullName () const {
        if (group_.isEmpty()) {
            return name_;
        } else {
            return QString ("%1/%2")
                    .arg(group_)
                    .arg(name_);
        }
    }


    //! The group.
    ///
    inline const QString &
    group () const {
        return group_;
    }

    //! Change the group.
    ///
    inline void
    setGroup (const QString & value) {
        group_ = value;
    }

    //! The description.
    ///
    inline const QString &
    description () const {
        return description_;
    }

    //! Change the description.
    ///
    inline void
    setDescription (const QString & value) {
        description_ = value;
    }

    //! The default.
    ///
    inline const QStringList &
    defaultValue () const {
        return default_;
    }

    //! Change the default.
    ///
    inline void
    setDefault (const QStringList & value) {
        default_ = value;
    }

    //! Is this value required to exist in at least one config file or not.
    ///
    inline bool
    required () const {
        return required_;
    }

    //! Change the required state.
    ///
    inline void
    setRequired (bool value) {
        required_ = value;
    }

protected:


private:


};

inline bool operator== (
        const OneOpt& lhs, const OneOpt& rhs){
    return lhs.name() == rhs.name(); }

inline bool operator!= (
        const OneOpt& lhs, const OneOpt& rhs){
    return lhs.name() != rhs.name(); }

#endif // GUARD_APPOPTS_ONEOPT_H_INCLUDE
