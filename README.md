# Raspberry
树莓派学习

wifi_connect.c提供自动连接wifi的功能，前提是你提供附近已知的wifi账号和密码，可自行修改
不具有wifi破解功能

使用:
    用于Linux系统中，主要是为了树莓派无显示屏、无TTL连接线时使用
    可写入系统启动时运行程序，程序首先检测自身网络连接状态，若无网络连接便会自行搜索周围可用wifi，输出你需要的wifi（你所给与的搜索条件）
    找到可用wifi时修改wifi连接文件，也可使程序通过使用命令连接（未添加功能，可自行解决），然后重启系统。
  
注：使用前需开放/etc/wpa_supplicant/wpa_supplicant.conf文件读写权限  
