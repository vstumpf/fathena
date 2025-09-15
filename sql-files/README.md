# SQL Imports

## Explanation of SQL import files

The files in this directory are basic SQL table building scripts. The contained scripts are needed for initial installs as well as subsequent updates.

### New Install
---
For a new install, the following needs to be imported into the main schema:
Note: The schema name is defined in `conf/inter_athena.conf::map_server_db`.

* main.sql - Contains tables for normal server usage.
* web.sql - Contains tables for the web service
* roulette_default_data.sql - Contains data for the client's roulette game.

For a new install, the following can be imported into the main schema but is highly suggested to be imported into a separate schema for logs:
Note: The schema name is defined in `conf/inter_athena.conf::log_db_db`.

* logs.sql - Contains tables for logging of server events.

### Updates
---
Over the course of time new features and optimizations will take place. This may require SQL changes to happen. In the `upgrades` folder will be SQL files with an attached date.
These only have to executed one time if an update has occurred after the initial installation. It's possible to see when an update may be required when a SQL error will be displayed on the server console stating the format differs from what is required.

### Notes
---
The `web-server` must be able to read the `login` and `guild` tables from the `login-server` and `char-server`, respectively.
