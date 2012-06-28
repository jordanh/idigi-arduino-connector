SET MQX_DIR="C:\Freescale\Freescale MQX 3.8\iDigiConnector"
SET BASE_DIR=..\..\..

RMDIR /S /Q %MQX_DIR%

MKDIR %MQX_DIR%
MKDIR %MQX_DIR%\source
MKDIR %MQX_DIR%\examples
MKDIR %MQX_DIR%\source\private
MKDIR %MQX_DIR%\source\include
MKDIR %MQX_DIR%\source\platform

COPY %BASE_DIR%\private\*.* %MQX_DIR%\source\private\
COPY %BASE_DIR%\public\include\*.h %MQX_DIR%\source\platform\
COPY %BASE_DIR%\public\run\platforms\freescale\*.* %MQX_DIR%\source\platform\
COPY ..\source\*.c %MQX_DIR%\source\platform\
COPY ..\include\*.h %MQX_DIR%\source\include\
COPY ..\..\common\source\*.c %MQX_DIR%\source\platform\
cOPY ..\..\common\include\*.h %MQX_DIR%\source\include\
COPY %BASE_DIR%\public\run\samples\simple_remote_config\config.rci %MQX_DIR%\source\platform\
COPY %BASE_DIR%\public\run\samples\simple_remote_config\remote_config.h %MQX_DIR%\source\platform\
COPY %BASE_DIR%\public\run\samples\simple_remote_config\remote_config_cb.c %MQX_DIR%\source\platform\
COPY %BASE_DIR%\public\run\samples\simple_remote_config\remote_config_cb.h %MQX_DIR%\source\platform\
COPY %BASE_DIR%\public\run\samples\simple_remote_config\system.c %MQX_DIR%\source\platform\
COPY %BASE_DIR%\public\run\samples\simple_remote_config\gps_stats.c %MQX_DIR%\source\platform\ 
