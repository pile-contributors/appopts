AppOpts
=======

The pile provides the application with easy loading
of options from config files, management and
saving the values back to config files.

The implementation looks into three locations
for configuration files:

- (1) system data directory
- (2) user home directory
- (3) current directory

If a value is present in more than one file the most
specific one (highest number) takes precedence.

The user may also choose to load the configuration from
a specific file or files using [loadFile].

Definitions
-----------

Some rudimentary support for option definitions exists
in the pile in the form of [oneopt] and [oneoptlist].
Through it one may provide a name, a group, a
description and a default value for each option.
Options may also be required to be present in at least
one config file.

[readMultipleFromCfgs] can be used to validate such
a list of options.

Usage
-----

A project using this pile would include an instance
of [appopts] in top level singleton:

    class Manager {
        AppOpts opts_; /**< the options */
        static Manager * unique_;
    };

It would then load the settings from all files
that are present with `opts_.loadFromAll ();`.

The code may go on parsing the command line and
overriding the values based on user request.

Dependencies
------------

- Qt (either 4.X or 5.X)
- UserMsg pile
- Perst pile

[appopts]: @ref AppOpts "AppOpts"
[loadFile]: @ref AppOpts::loadFile "loadFile()"
[readMultipleFromCfgs]: @ref AppOpts::readMultipleFromCfgs "readMultipleFromCfgs()"
[oneopt]: @ref OneOpt "OneOpt"
[oneoptlist]: @ref OneOptList "OneOptList"
