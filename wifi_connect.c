//替换文件内容:wifi账号、密码
/**
 * wifi账号与密码对应表单
 * buff存储        完整账号             密码
1* A508            A508             233231004
2* 2403            2403                 dfhc123456
3*
4*
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/wireless.h>

int get_wireless_status(char *ath)
{
	int sock_fd;
	int ret = 0;
	struct iwreq iwr;
	struct iw_statistics stats;

	if (ath == NULL)
	{
		printf("ath is NULL\n");
		return -1;
	}

	/* socket fd */
	if ((sock_fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
	printf("socket err\n");
	return -1;
	}

	memset(&iwr, 0, sizeof(iwr));
	memset(&stats, 0, sizeof(stats));
	iwr.u.data.pointer = (caddr_t) &stats; /* result value */
	iwr.u.data.length = sizeof(stats);
	iwr.u.data.flags = 1; /* clear updated flag */

	strncpy(iwr.ifr_ifrn.ifrn_name, ath, IFNAMSIZ - 1);

	if (ioctl(sock_fd, SIOCGIWSTATS, &iwr) < 0)
	{
		printf("No Such Device %s\n",ath);
		close(sock_fd);
		return -1;
	}

	ret = stats.status;

	close(sock_fd);
	return ret;
}

//从附近查找需要的wifi账号
void search(char buff[30])
{
	FILE   *stream;
	char   buf[1024];
	int    x = 0;
	int    yes_value = 0;

    	memset(buf, '\0', sizeof(buf));//初始化buf,以免后面写如乱码到文件中
    	stream = popen("iwlist wlan0 scan | grep ESSID", "r"); //通过管道读取（“r”参数）到FILE* stream

    	fread(buf, sizeof(char), sizeof(buf), stream); //将刚刚FILE* stream的数据流读取到buf中

	for(int i = 0; i < 1024; i++)
	{
		if(buf[i] == 'D' && buf[i+1] == ':' && buf[i+2] == '"')
		{
			for(int j = 0;j < 1024; j++)
			{
				if(buf[i+3+j] == 'A' && buf[i+3+j+1] == '5' && buf[i+3+j+2] == '0')
					yes_value = 1;
				else if(buf[i+3+j] == '2' && buf[i+3+j+1] == '4' && buf[i+3+j+2] == '0')
					yes_value = 1;

				if(yes_value == 1 && buf[i+3+j] != '"')
				{
					buff[x] = buf[i+3+j];
					x++;
				}
				else if(buf[i+3+j] == '"')
				{
					buff[x] = '\0';
					break;
				}
			}
		}

		if(yes_value == 1)
			break;
	}

		pclose(stream);
}

void replace(char replace_id[], char replace_psk[])
{
	int Len_name, Len_pwd;
	for(int x = 0; x < 20; x++)
	{
		if(replace_id[x] == '\0')
		{
			Len_name = x+1;
			break;
		}
	}
	for(int x = 0; x < 20; x++)
	{
		if(replace_psk[x] == '\0')
		{
			Len_pwd = x+1;
			break;
		}
	}

	int offset = 0;
	int off_left = 0,off_right = 0;
	FILE *fp = fopen("/etc/wpa_supplicant/test.conf", "r+");
	if(fp == NULL)
	{
		printf("file error\n");
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	int flen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char buff[flen];
	memset(buff, '\0', sizeof(buff));

	fread(buff, flen, 1, fp);

	for(int i = 0; i < flen; i++)
	{
		if(buff[i-1] == 's' && buff[i] == 'i' && buff[i+1] == 'd')
		{
			off_left = i+4;
			for(int j = 0; j < 10; j++)
			{
				if(buff[off_left+j] == '"')
				{
					off_right = off_left+j;
					break;
				}
			}
			offset = off_right - off_left;

			char buffer[flen + (Len_name - 1 - offset)];
			memset(buffer, '\0', sizeof(buffer));
			for(int a = 0; a < flen; a++)
			{
				buffer[a] = buff[a];
			}

			//字符串替换
			if(offset < Len_name-1)
			{
				for(int x = sizeof(buffer)-1; x >= off_right; x--)
				{
					buffer[x+(Len_name-1-offset)] = buffer[x];
				}
				for(int y = 0; y < Len_name-1; y++)
				{
					buffer[off_left+y] = replace_id[y];
				}
			}
			else if(offset > Len_name-1)
			{
				for(int x = off_right; x <= sizeof(buffer); x++)
				{
					buffer[x-(offset-Len_name+1)] = buffer[x];
				}
				for(int y = 0; y < Len_name-1; y++)
				{
					buffer[off_left+y] = replace_id[y];
				}
			}
			else
			{
				for(int y = 0; y < offset; y++)
				{
					buffer[off_left+y] = replace_id[y];
				}
			}

			system(":> /etc/wpa_supplicant/test.conf");
			fseek(fp, 0, SEEK_SET);
			fwrite(buffer, sizeof(buffer), 1, fp);
		}

		fseek(fp, 0L, SEEK_END);
		flen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char buf[flen];
		memset(buf, '\0', sizeof(buf));

		fread(buf, flen, 1, fp);

		if(buf[i-1] == 'p' && buf[i] == 's' && buf[i+1] == 'k')
		{
			off_left = i+4;
			for(int j = 0; j < 10; j++)
			{
				if(buf[off_left+j] == '"')
				{
					off_right = off_left+j;
					break;
				}
			}
			offset = off_right - off_left;

			char buffer[flen + (Len_pwd - 1 - offset)];
			memset(buffer, '\0', sizeof(buffer));
			for(int a = 0; a < flen; a++)
			{
				buffer[a] = buf[a];
			}

			//字符串替换
			if(offset < Len_pwd-1)
			{
				for(int x = sizeof(buffer)-1; x >= off_right; x--)
				{
					buffer[x+(Len_pwd-1-offset)] = buffer[x];
				}
				for(int y = 0; y < Len_pwd-1; y++)
				{
					buffer[off_left+y] = replace_psk[y];
				}
			}
			else if(offset > Len_pwd-1)
			{
				for(int x = off_right; x <= sizeof(buffer); x++)
				{
					buffer[x-(offset-Len_pwd+1)] = buffer[x];
				}
				for(int y = 0; y < Len_pwd-1; y++)
				{
					buffer[off_left+y] = replace_psk[y];
				}
			}
			else
			{
				for(int y = 0; y < offset; y++)
				{
					buffer[off_left+y] = replace_psk[y];
				}
			}

			system(":> /etc/wpa_supplicant/wpa_supplicant.conf");
			fseek(fp, 0, SEEK_SET);
			fwrite(buffer, sizeof(buffer), 1, fp);
		}
	}
}

int main(int argc, char** argv)
{
	char	buff[30];
	int 	value;

	memset(buff, '\0', sizeof(buff));
	value = get_wireless_status("wlan0");
	if(value == 1)
	{
		search(buff);//查看附近wifi，并输出自己可连接wifi到buff中
   		//printf("%s\n", buff);

		//连接wifi，账号存在buff中
		if(buff[0] == 'A' && buff[1] == '5' && buff[2] == '0' && buff[3] == '8')
		{
			//账号：广安A508
			char Name[] = "A508";
			char Pwd[]  = "23231004";
			replace(Name, Pwd);
		}
		else if(buff[0] == '2' && buff[1] == '4' && buff[2] == '0' && buff[3] == '3')
		{
			//账号：2403
			char Name[] = "2403";
			char Pwd[]  = "dfhc123456";
			replace(Name, Pwd);
		}
	}

	return 0;
}
