# rAthena Tools

As with any product, please make a backup of your database directory before continuing with any of these tools listed below.

## Mapcache

The mapcache tool will allow you to generate or update the map_cache.dat that is located in `db/`. Simply add the GRF or Data directories that contain the `.gat` and `.rsw` files to the `conf/grf-files.txt` before running.

## YAMLUpgrade

There are times when a YAML database may have to go through a restructure. When running your server, you will be prompted with:

> Database version # is not supported anymore. Minimum version is: #

Simply run the YAMLUpgrade tool and when prompted to upgrade said database, let the tool handle the conversion for you!
