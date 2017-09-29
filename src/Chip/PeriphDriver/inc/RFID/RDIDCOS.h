#ifndef RFIDCOS_H 
#define RFIDCOS_H 

/*外部认证	\x00\x82
	P2:外部认证密钥标识号
	xxxxxxxx:8字节加密后的随机数
	\x00\x82\x00\P2\x08\xxxxxxxx
*/
#define xRFCosCmd_ExternalAuthenticion "\x00\x82\x00"

/*内部认证	 \x00\x88
	P1: \x00,加密; \x01,解密; \x02,Mac计算
	P2:DES密钥标识号
	Lc:认证数据长度
	Data:认证的数据
	\x00\x88\P1\P2\Lc\Data
*/
#define xRFCosCmd_InternalAuthenticion "\x00\x88"

/*取随机数
	Le:随机数长度
	\x00\x84\x00\x00\Le
*/
#define xRFCosCmd_GetChallenge			 "\x00\x84\x00\x00\x08"

/*选择文件	 \x00\xA4
	P1: \x00,文件标识符选择，选择当前目录下基本文件或子目录文件;
	    \x01, 用目录名称选择
	P2:DES密钥标识号
	Lc:认证数据长度
	Data:认证的数据
	\x00\xA4\P1\x00\Lc\Data\x00
*/
#define xRFCosCmd_Select "\x00\xA4"

/*擦除目录文件
	"\x80\x0E\x00\x00\x00"
*/
#define xRFCosCmd_EraseDF			 "\x80\x0E\x00\x00\x00"


#endif

