
#include <com_judge.h>
#include <iostream>

#define u8 uint8_t
#define COLOR 100 // 100 = blue,0 = red

static uint8_t SEQ_num = 0;//发送包序号，一般从0开始递增
static int errNum = 0;//写串口的错误序号,我们期望它一直为0

/************************************֡ͷCRC8У��**************************************************************/
//crc8 generator polynomial:G(x)=x8+x5+x4+1
const unsigned char CRC8_INIT = 0xff;
const unsigned char CRC8_TAB[256] =
		{
				0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
				0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
				0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
				0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
				0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
				0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
				0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
				0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
				0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
				0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
				0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
				0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
				0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
				0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
				0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
				0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
		};

		
/*
** Descriptions: CRC8 checksum function
** Input: Data to check,Stream length, initialized checksum
** Output: CRC checksum
*/
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8)
{
		unsigned char ucIndex;
		while (dwLength--)//Stream length
		{
			ucIndex = ucCRC8^(*pchMessage++);//^λ����:���
			ucCRC8 = CRC8_TAB[ucIndex];
		}
		return(ucCRC8);//���
}

/*
** Descriptions: CRC8 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/

unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
		unsigned char ucExpected = 0;
		if ((pchMessage == 0) || (dwLength <= 2)) return 0;
		ucExpected = Get_CRC8_Check_Sum (pchMessage, dwLength-1, CRC8_INIT);
		return ( ucExpected == pchMessage[dwLength-1] );
}

/*
** Descriptions: append CRC8 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/

u8 Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
		unsigned char ucCRC = 0;
		if ((pchMessage == 0) || (dwLength <= 2)) return 0;
		ucCRC = Get_CRC8_Check_Sum ( (unsigned char *)pchMessage, dwLength-1, CRC8_INIT);
		pchMessage[dwLength-1] = ucCRC;
		return ucCRC;
}


/************************************����βCRC16У��**************************************************************/
uint16_t CRC_INIT = 0xffff;
const uint16_t wCRC_Table[256] =
		{
				0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
				0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
				0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
				0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
				0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
				0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
				0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
				0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
				0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
				0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
				0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
				0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
				0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
				0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
				0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
				0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
				0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
				0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
				0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
				0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
				0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
				0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
				0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
				0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
				0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
				0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
				0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
				0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
				0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
				0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
				0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
				0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
		};

/*
** Descriptions: CRC16 checksum function
** Input: Data to check,Stream length, initialized checksum
** Output: CRC checksum
*/

uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
		uint8_t chData;
		if (pchMessage == nullptr)
		{
			return 0xFFFF;
		}
		while(dwLength--)
			{
				chData = *pchMessage++;
				(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^(uint16_t)(chData)) & 0x00ff];
			}
		return wCRC;
}

/*
** Descriptions: CRC16 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/

uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
		uint16_t wExpected = 0;
		if ((pchMessage == nullptr) || (dwLength <= 2))
		{
		return CRC_Check_False;
		}
		wExpected = Get_CRC16_Check_Sum ( pchMessage, dwLength - 2, CRC_INIT);
		return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff)
		== pchMessage[dwLength - 1]);
}
/*
** Descriptions: append CRC16 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
uint16_t Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength)
{
		uint16_t wCRC = 0;
		if ((pchMessage == nullptr) || (dwLength <= 2))
		{
		return 0;
		}
		wCRC = Get_CRC16_Check_Sum ( (u8 *)pchMessage, dwLength-2, CRC_INIT );
		pchMessage[dwLength-2] = (u8)(wCRC & 0x00ff);
		pchMessage[dwLength-1] = (u8)((wCRC >> 8)& 0x00ff);
		wCRC=pchMessage[dwLength-2]<<8 | pchMessage[dwLength-1];
		return wCRC;
}

JudgeSerial::JudgeSerial()
{
	std::string serial_port_ = "/dev/ttyUSB0";
	device_ = std::make_shared<roborts_sdk::SerialDevice>(serial_port_, 115200);
}

bool JudgeSerial::Init()
{
	if (!device_->Init())
	{
		LOG_ERROR << "Can not open device: "
				  << ". Please check if the USB device is inserted and connection is configured correctly!";
		return false;
	}
	LOG_INFO << "Connection to serial_port_";
	return true;
}

/**
 * @brief 小地图交互函数，本函数高度集成了串口协议数据包的编写与串口通信的发送操作
 * 
 * @param robo_ID 需要标记的机器人id，蓝色+100
 * @param target_x 目标在小地图上的位置（单位：m）
 * @param target_y 目标在小地图上的位置（单位：m）
 * @param Judge 裁判系统Serial
 */
void Radar_Send2MiniMap(uint16_t robo_ID, float target_x, float target_y, JudgeSerial Judge)
{

	uint8_t *fpoint1, *fpoint2;
	UI_Packhead head;
	Minimap_t minimap_1;
	fpoint1 = (uint8_t *)&minimap_1;
	head.SOF = 0xA5;
	head.Data_length = 14;      //此处依然为计算保留位（id.2byte+targetx.4byte+targety.4byte+reserved.4byte）的写法，若官方修好了bug应当是10
	head.Seq = SEQ_num++;

	memcpy(&minimap_1.Minimap, &head, 4);
	head.CRC8 = Get_CRC8_Check_Sum(minimap_1.Minimap, 4, 0xff);  //帧头CRC8校验
	memset(&minimap_1.Minimap, 0, sizeof(minimap_1.Minimap));
	head.CMD_ID = 0x0305;
	memcpy(fpoint1, &head, 7);

	ext_client_map_command_t *Location = new ext_client_map_command_t;

	Location->target_robot_ID = robo_ID;
	Location->target_position_x = target_x;
	Location->target_position_y = target_y;
	Location->reserverd = 0.0;

	fpoint2 = (unsigned char *)&Location->target_robot_ID;
	memcpy(fpoint1 + sizeof(head), fpoint2, 2);

	fpoint2 = (unsigned char *)&Location->target_position_x;
	memcpy(fpoint1 + sizeof(head) + sizeof(Location->target_robot_ID), fpoint2, 4);

	fpoint2 = (unsigned char *)&Location->target_position_y;
	memcpy(fpoint1 + sizeof(head) + sizeof(Location->target_robot_ID) + sizeof(Location->target_position_x), fpoint2, 4);

	fpoint2 = (unsigned char *)&Location->reserverd;
	memcpy(fpoint1 + sizeof(head) + sizeof(Location->target_robot_ID) + sizeof(Location->target_position_x) + sizeof(Location->target_position_y), fpoint2, 4);

	Append_CRC16_Check_Sum(minimap_1.Minimap, 23); //整包CRC16校验

	uint8_t *p = (uint8_t *)&minimap_1.Minimap;

	if (Judge.device_->Write(p, 23) < 0)
	{
		errNum++;
		std::cout << "WRITE ERROR " << errNum << std::endl;
	}
	else
	{
		std::cout << "WRITE SUCCESS" << std::endl;
	}
}
/**
 * @brief 这个函数作为发送预警信息使用，是EXcontrol函数的一个范例
 * 
 * @param robo_ID 目标机器人id(操作手客户端)
 * @param msg 需要发送的内容，uint8
 * @param Judge 裁判系统
 */
void Radar_SendWarningMsg(uint16_t robo_ID, uint8_t *msg, JudgeSerial Judge)
{
	UI_Packhead *frame_head = new UI_Packhead;
	uint8_t *com_msg = new uint8_t[100];

	frame_head->SOF = 0xA5;
	frame_head->Data_length = 6 + 45;
	frame_head->Seq = SEQ_num++;

	memcpy(com_msg, frame_head, 4);
	frame_head->CRC8 = Get_CRC8_Check_Sum(com_msg, 4, 0xff);
	frame_head->CMD_ID = 0x0301;
	memset(com_msg, 0, sizeof(com_msg));
	memcpy(com_msg, frame_head, 7);

	ext_student_interactive_header_data_t *data_head = new ext_student_interactive_header_data_t;
	data_head->data_cmd_id = 0x0110;
	data_head->sender_ID = COLOR + 9;
	data_head->receiver_ID = COLOR + robo_ID;

	memcpy(com_msg + 7, data_head, 6);

	//配置字符信息
	graphic_data_struct_t *graphic_data = new graphic_data_struct_t;
	graphic_data->graphic_name[0] = 'w';
	graphic_data->graphic_name[1] = 'a';
	graphic_data->graphic_name[2] = 'n';

	graphic_data->operate_tpye = 1;
	graphic_data->graphic_tpye = 7;
	graphic_data->layer = 9;
	graphic_data->color = 3;
	//字体大小
	graphic_data->start_angle = 20;
	//字符长度
	graphic_data->end_angle = 30;
	//线宽
	graphic_data->width = 2;
	//起点xy坐标
	graphic_data->start_x = 13;
	graphic_data->start_y = 1;

	//图形结构体其余置空即可
	//填充图形设置
	memcpy(com_msg + 7 + 6, graphic_data, 15);
	//填充发送字符
	memcpy(com_msg + sizeof(UI_Packhead) + sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t), msg, 30);
	//两种写法都可以，只要字节偏移量正确
	//写串口
	if (Judge.device_->Write(com_msg, 7 + 6 + 15 + 30) < 0)
	{
		errNum++;
		std::cout << "WRITE ERROR " << errNum << std::endl;
	}
	else
	{
		std::cout << "WRITE SUCCESS" << std::endl;
	}
}
/**
 * @brief 这个函数是通过裁判系统串口通信进行客户端图形操作的集成函数，详见裁判系统串口协议，此函数根据v1.3编写,上行频率最大为10hz
 * 		  （目前仅支持单目标单图形的操作0.0,示例可见发送预警信息函数）Author:Windbell
 * 
 * @param robo_ID 接收者，即目标操作手客户端所对应的机器人ID(关于颜色，本代码已经根据COLOR宏定义统一管理)
 *    			  ps:机器人ID：1，英雄(红)；2，工程(红)；3/4/5，步兵(红)；6，空中(红)；7，哨兵(红)；9，雷达站（红）；
 * 				  101，英雄(蓝)；102，工程(蓝)；103/104/105，步兵(蓝)；106，空中(蓝)；107，哨兵(蓝)； 109，雷达站（蓝）
 * @param target_x 图形或字符的起点x坐标，对于正圆、椭圆、圆弧，此项为圆心坐标
 * @param target_y 图形或字符的起点y坐标，对于正圆、椭圆、圆弧，此项为圆心坐标
 * @param name 所画图形的索引名，注意要<3byte
 * @param optype 操作数,bit 0-2：图形操作：0：空操作;1：增加；2：修改；3：删除.默认6表示空操作
 * @param gtype 图形类型,Bit 3-5：图形类型：0：直线；1：矩形；2：整圆；3：椭圆；4：圆弧；5：浮点数；6：整型数；7：字符；
 * @param oplayer 操作图层，客户端共9个图层，0~9，占Bit 6-9
 * @param gcolor 图形颜色，Bit 10-13：颜色：0：红蓝主色；1：黄色；2：绿色；3：橙色；4：紫红色；5：粉色；6：青色；7：黑色；8：白色；
 * @param g_startangle 仅对圆弧、字符(浮点、整形、字符)有效，分别代表起始角度和字体大小，其余没有影响
 * @param g_endangle 仅对圆弧、浮点数、字符有效，分别代表终止角度、小数位有效个数、字符长度，其余没有影响
 * @param gwidth 线条宽度，对所有图形均有效，代表线条宽度，其中字符建议字体大小与线条长度之比取10:1
 * @param gradius 半径，仅对正圆、浮点、整形有效；当类型为浮点、整形时，将gradius+gendx+gendy整合作为数据内容处理，int32_t(浮点数*1000转换成int32_t)
 * @param gendx x坐标，对直线、矩形、椭圆、圆弧有效,分别代表终点x坐标、对角x坐标、x半轴长、x半轴长
 * @param gendy y坐标，对直线、矩形、椭圆、圆弧有效,分别代表终点y坐标、对角y坐标、y半轴长、y半轴长
 * @param extra_data 仅对字符有效，代表字符内容
 * @param Judge 裁判系统
 */
// void Radar_EX_ControlGraphicOnTargetClient(JudgeSerial Judge, uint16_t robo_ID, uint32_t target_x=0, uint32_t target_y=0, uint8_t * name=nullptr, uint32_t optype=6, uint32_t gtype=0, uint32_t oplayer=0, uint32_t gcolor=0, uint32_t g_startangle=0, uint32_t g_endangle=0, uint32_t gwidth=0, uint32_t gradius=0, uint32_t gendx=0, uint32_t gendy=0, uint8_t * extra_data = nullptr)
// {
// 	UI_Packhead *frame_head = new UI_Packhead;
// 	uint8_t *com_msg = new uint8_t[128];
// 	int msg_length = 0;
// 	frame_head->SOF = 0xA5;

// 	if (gtype == 7)
// 		msg_length = frame_head->Data_length = 6 + 45;
// 	else
// 		msg_length = frame_head->Data_length = 6 + 15;

// 	frame_head->Seq = SEQ_num++;

// 	memcpy(com_msg, frame_head, 4);
// 	frame_head->CRC8 = Get_CRC8_Check_Sum(com_msg, 4, 0xff);
// 	frame_head->CMD_ID = 0x0301;
// 	memset(com_msg, 0, sizeof(com_msg));
// 	memcpy(com_msg, frame_head, 7);

// 	ext_student_interactive_header_data_t *data_head = new ext_student_interactive_header_data_t;
// 	if(gtype == 7)
// 		data_head->data_cmd_id = 0x0110;
// 	else 
// 		data_head->data_cmd_id = 0x0101;
// 	data_head->sender_ID = COLOR + 9;
// 	data_head->receiver_ID = COLOR + robo_ID;

// 	memcpy(com_msg + 7, data_head, 6);

// 	//配置信息
// 	graphic_data_struct_t *graphic_data = new graphic_data_struct_t;
// 	for (size_t i = 0; i < 3; i++)
// 	{
// 		graphic_data->graphic_name[i] = name[i];
// 	}
	
// 	graphic_data->operate_tpye = optype;
// 	graphic_data->graphic_tpye = gtype;
// 	graphic_data->layer = oplayer;
// 	graphic_data->color = gcolor;
// 	graphic_data->start_angle = g_startangle;
// 	graphic_data->end_angle = g_endangle;
// 	graphic_data->width = gwidth;
// 	graphic_data->start_x = target_x;
// 	graphic_data->start_y = target_y;

// 	//填充图形设置
// 	memcpy(com_msg + 7 + 6, graphic_data, 15);
// 	if(gtype == 7)
// 		//填充发送字符
// 		memcpy(com_msg + sizeof(UI_Packhead) + sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t), extra_data, 30);

// 	//写串口
// 	if (Judge.device_->Write(com_msg, 7 + 6 + 15 + 30) < 0)
// 	{
// 		errNum++;
// 		std::cout << "WRITE ERROR " << errNum << std::endl;
// 	}
// 	else
// 	{
// 		std::cout << "WRITE SUCCESS" << std::endl;
// 	}
// }