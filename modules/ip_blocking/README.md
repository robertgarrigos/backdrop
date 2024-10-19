IP Address Blocking
===================

Allows you to block IP addresses.
This module restores the lost functionality from the Drupal core.
It also comes with a number of improvements.

Options available:

  - see when an IP was blocked, who blocked that IP and the reason for the block (if specified);
  - set a 404 (Not Found) status code for visitors from blocked IPs instead of the default 403 (Access Denied);
  - enable logging of access attempts from blocked IPs;
  - from the "Recent log messages" event pages (admin/reports/event/EVENT_NUMBER) you can easily block or unblock an IP
    and check the status of this IP on the [AbuseIPDB](https://www.abuseipdb.com);
  - view of the number of blocked IPs on the "Status report" page;
  - get integration with the "Antiscan" module (https://backdropcms.org/project/antiscan) to
    automatically block IP addresses used by bad crawlers or vulnerability scanners;
  - use the "AbuseIPDB Report" module (https://backdropcms.org/project/abuseipdb_report) to send
    manual or automatic reports to the [AbuseIPDB](https://www.abuseipdb.com/) database.

Installation
------------
Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules

Note: If you have the "Ban IP" module installed, you will need to uninstall it first.

Configuration and usage
-----------------------
The administration page is available from the *Administration > Configuration >
User accounts > IP address blocking* menu (admin/config/people/ip-blocking)
and can be used to:

- block an IP address:
    - enter a valid IP address (e.g. 10.0.0.1);
    - (optional) enter description of the reason for blocking this IP;
    - click Add;

- unblock or check status a previously blocked IP address:
    - in the table below the form, click "unblock" for the IP address in the "Operation" column, and then confirm the unblock;
    - In the same column click "check" to check the status of this IP on [AbuseIPDB](https://www.abuseipdb.com) (opens in a new tab).

While browsing "Recent log messages" (admin/reports/dblog) you can quickly review
an individual entry (admin/reports/event/EVENT_NUMBER) and block (or unblock)
an IP address and check the status of this IP on [AbuseIPDB](https://www.abuseipdb.com) from the "Operation" links.

This link will be displayed for 'access denied', 'antiscan', 'ip_blocking', 'login_allowlist', 'search',
'page not found', 'system', 'user', 'php' and 'ajax' event types if the event has a valid IP address and not the IP address of the currently logged in user.

**Screenshots** are available at https://findlab.net/projects/ip-address-blocking

Known issues
------------
Sometimes you may see in log messages like this:
> "Warning: Cannot modify header information - headers already sent by (output started at /core/includes/bootstrap.inc:3132) in ip_blocking_boot() (line 113 of /modules/contrib/ip_blocking/ip_blocking.module)"

Here is the explanation.

When Backdrop served a cached page, the 'X-Backdrop-Cache: HIT' and 'cache-control' headers were sent with the obsolete entries before they were actually generated for the request.

To avoid such messages and incorrect module actions (in such cases can not get in time to reject blocked IP) you have two options:

  - you can disable prefetching for cached pages: go to 'admin/config/development/performance' and within the 'Caching' fieldset uncheck the 'Use background fetch for cached pages' checkbox, then press the 'Save configuration' button;
  - add the option '$settings['page_cache_invoke_hooks'] = TRUE;' to your 'settings.php' file.

Disabling prefetching for cached pages (first option) is sufficient to avoid such collisions in most cases.

Note for Drupal 7 migration
---------------------------
If you install this module on a site that has been upgraded from Drupal 7 and has blocked IPs (in the 'blocked_ips' table),
this module will rename that table to 'blocked_ips_d7') and create its own 'blocked_ips' table with 4 additional columns: 'reason', 'uid, 'time' and 'type'.

After installation, you can re-import previously blocked IPs from the 'blocked_ips_d7' table,
or delete this table if you do not need it by using the link on the "Status Report" page
or by going to 'admin/config/people/ip-blocking/orphaned_table'.

License
-------
This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainer
------------------
Vladimir (https://github.com/findlabnet/)

More information
----------------
For bug reports, feature or support requests, please use the module
issue queue at https://github.com/backdrop-contrib/ip_blocking/issues.
