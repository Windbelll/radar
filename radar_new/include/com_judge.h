#ifndef __COM_JUDGE_H
#define __COM_JUDGE_H

#define CRC_Check_False 0
#define CRC_Check_True 1

#include <serial_device.h>
#include <hardware_interface.h>
#include "../utilities/log.h"
#include "stdbool.h"
#include "stdint.h"
#include <memory>
#include <string>
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint8_t Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);

uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
uint16_t Append_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);

class JudgeSerial
{
public:
	JudgeSerial();
	// ~JudgeSerial();
	bool Init();

	// std::string serial_port_;
	std::shared_ptr<roborts_sdk::SerialDevice> device_;
};

/* ID: 0x0305  Byte: 14  Radar communication of minimap*/
/*小地图接收信息标识：0x0305。最大接收频率：10Hz。雷达站发送的坐标信息可以被所有己方操作手在第一视角小地图看到。*/
typedef struct
{
	uint16_t target_robot_ID;
	float target_position_x;
	float target_position_y;
	float reserverd; //经典bug保留位
} __attribute__((__packed__)) ext_client_map_command_t;

typedef struct
{
	uint8_t SOF;
	uint16_t Data_length;
	uint8_t Seq;
	uint8_t CRC8;
	uint16_t CMD_ID;
} __attribute__((__packed__)) UI_Packhead;

typedef struct
{
	uint8_t data_t[10];
} __attribute__((__packed__)) Minimap_data_t;

typedef struct
{
	uint8_t Minimap[23];
} __attribute__((__packed__)) Minimap_t;

typedef struct
{
	uint8_t graphic_name[3];
	uint32_t operate_tpye : 3;
	uint32_t graphic_tpye : 3;
	uint32_t layer : 4;
	uint32_t color : 4;
	uint32_t start_angle : 9;
	uint32_t end_angle : 9;
	uint32_t width : 10;
	uint32_t start_x : 11;
	uint32_t start_y : 11;
	uint32_t radius : 10;
	uint32_t end_x : 11;
	uint32_t end_y : 11;
} __attribute__((__packed__)) graphic_data_struct_t;

typedef struct
{
	uint16_t data_cmd_id;
	uint16_t sender_ID;
	uint16_t receiver_ID;
} __attribute__((__packed__)) ext_student_interactive_header_data_t;

void Radar_Send2MiniMap(uint16_t robo_ID, float target_x, float target_y, JudgeSerial Judge);

// void Radar_EX_ControlGraphicOnTargetClient(JudgeSerial Judge, uint16_t robo_ID, uint32_t target_x, uint32_t target_y, uint8_t * name=nullptr, uint32_t optype=6, uint32_t gtype=0, uint32_t oplayer=0, uint32_t gcolor=0, uint32_t g_startangle=0, uint32_t g_endangle=0, uint32_t gwidth=0, uint32_t gradius=0, uint32_t gendx=0, uint32_t gendy=0, uint8_t * extra_data = nullptr);

void Radar_SendWarningMsg(uint16_t robo_ID, uint8_t *msg, JudgeSerial Judge);
#endif
