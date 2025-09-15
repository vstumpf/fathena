#!/usr/bin/env bash

function aborterror {
	echo $@
	exit 1
}

# Github Actions default login data
DB_ROOT=root
DB_ROOTPW=root
# rAthena default MySQL data
DB_HOST=localhost
DB_NAME=ragnarok
DB_USER=ragnarok
DB_PASS=ragnarok

mysql -u $DB_ROOT -p$DB_ROOTPW -e "CREATE DATABASE $DB_NAME;" || aborterror "Unable to create database."
mysql -u $DB_ROOT -p$DB_ROOTPW $DB_NAME < sql-files/main.sql || aborterror "Unable to import main database."
mysql -u $DB_ROOT -p$DB_ROOTPW $DB_NAME < sql-files/logs.sql || aborterror "Unable to import logs database."
mysql -u $DB_ROOT -p$DB_ROOTPW $DB_NAME < sql-files/roulette_default_data.sql || aborterror "Unable to import roulette table."
# MariaDB
mysql -u $DB_ROOT -p$DB_ROOTPW -e "SET old_passwords=0; CREATE USER '$DB_USER'@'$DB_HOST' IDENTIFIED BY '$DB_PASS';"
# MySQL
mysql -u $DB_ROOT -p$DB_ROOTPW -e "CREATE USER '$DB_USER'@'$DB_HOST' IDENTIFIED WITH mysql_native_password BY '$DB_PASS';"
mysql -u $DB_ROOT -p$DB_ROOTPW -e "GRANT SELECT,INSERT,UPDATE,DELETE ON $DB_NAME.* TO '$DB_USER'@'$DB_HOST';"
