# ![User Import](https://github.com/backdrop-contrib/user_import/blob/1.x-3.x/images/user-import.png "User Import for BackdropCMS")

Import users into Backdrop from a CSV file (Comma Separated File).

**Features include:**

* Creates an account for each user
* Match CSV columns to profile fields.
* Can optionally use the file's first row to map CSV data to user profile fields
* Option to create Usernames based on data from file, e.g. "John" + "Smith" => "JohnSmith"
* Usernames can be made of abbreviated data from file, e.g. "Jane" + "Doe" => "JDoe"
* Option to create random, human readable, usernames
* Option to import passwords or create random passwords for each user
* Can set user roles
* Option to send welcome email, with account details to each new user
* Can set each user's contact form to enabled
* Test mode option to check for errors
* Processing can be triggered by cron or manually by an administrator
* Can stagger number of users imported, so that not too many emails are sent at one time
* Multiple files can be imported/tested at the same time
* Option to make new accounts immediately active, or inactive until user logs in
* Use CSV file already uploaded through FTP (useful for large imports)
* Designed to be massively scalable


## Installation and Usage

- Install this module using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules).
- A private file directory must be set (you will be prompted to do this if you haven't done so and attempt to import users).
- Usage instructions can be [viewed and edited in the Wiki](https://github.com/backdrop-contrib/user_import/wiki).

## Issues

 - Bugs and Feature requests should be reported in the [Issue Queue](https://github.com/backdrop-contrib/user_import/issues).

## Current Maintainers

 - [Laryn Kragt Bakker](https://github.com/laryn) - [CEDC.org](https://cedc.org)

## Credits

 - Ported to Backdrop by [Laryn Kragt Bakker](https://github.com/laryn) - [CEDC.org](https://cedc.org)
 - Created and maintained for Drupal by [Robert Castelo](http://drupal.org/user/3555) - [CodePositive](http://www.codepositive.com)
 - A script by [David McIntosh](neofactor.com) informed the development of this module.
 - Patches, documentation and code provided by community members including spatz4000, mfredrickson, idealso, and Nedio Rogers. 

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
