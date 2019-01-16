
::version.bat
:: creates file version.prop with two properties
::    date = <date time>
::    os = <os name>

echo date = %date:~-4,4%-%date:~-10,2%-%date:~-7,2% %time:~0,2%:%time:~3,2%>pps/version.prop
echo os = %os%>>pps/version.prop

:: sample output

:: date = 2018-05-24 14:58
:: os = Windows_NT

