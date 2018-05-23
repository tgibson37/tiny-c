#version.sh
# creates file version.prop with two properties
#    date = <date time>
#    os = <os name>
echo date = `date "+%Y-%m-%d %H:%M"` > pps/version.prop
echo os = `uname -o` >> pps/version.prop
