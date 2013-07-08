echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

echo Deleting files in \"output/\": 
rm `find output/* -type f | grep -v '^output/console-output.txt$'`
rm -r `find output/* -type d`

echo ---------------------------------------------------------------

echo Deleting files in \"status/\":
rm -r status/*

echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++