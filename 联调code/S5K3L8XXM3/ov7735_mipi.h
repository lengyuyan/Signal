#ifndef _OV7735_MIPI_
#define _OV7735_MIPI_
#include "DTCCM2_SDK\imagekit.h"
USHORT OV7735_MIPI_ParaList[] =
{
	0x0100, 0x00,
	0x0103, 0x01,
	0x0100, 0x00,
	0x0100, 0x00,
	0x0100, 0x00,
	0x0100, 0x00,
	0x3037, 0x02,
	0x3039, 0x00,
	0x3034, 0x18,
	0x3036, 0x40,
	0x3035, 0x41,
	0x3662, 0x10,
	0x3106, 0x09,
	0x3020, 0x31,
	0x3663, 0x01,
	0x3007, 0x80,
	0x303d, 0x20,
	0x303c, 0x11,
	0x303b, 0x08,
	0x303a, 0x00,
	0x3821, 0x10,
	0x3660, 0x90,
	0x3622, 0x00,
	0x3820, 0x00,
	0x3600, 0x98,
	0x3601, 0x4c,
	0x3602, 0x0e,
	0x3603, 0x24,
	0x3620, 0x53,
	0x3621, 0x10,
	0x3604, 0xc1,
	0x3631, 0x2a,
	0x3633, 0x65,
	0x3634, 0x01,
	0x3701, 0x33,
	0x3704, 0x28,
	0x370c, 0x30,
	0x370d, 0x03,
	0x3712, 0x05,
	0x3613, 0x00,
	0x3707, 0x67,
	0x3823, 0x00,
	0x4500, 0x32,
	0x5000, 0xcf,
	0x5484, 0x63,
	0x5485, 0x6e,
	0x5486, 0x78,
	0x5487, 0x80,
	0x5488, 0x88,
	0x5489, 0x90,
	0x5800, 0x00,
	0x5801, 0x00,
	0x5802, 0x01,
	0x5803, 0x40,
	0x5804, 0x00,
	0x5805, 0xf0,
	0x5806, 0x28,
	0x5807, 0x2a,
	0x5808, 0x3a,
	0x5308, 0x30,
	0x5306, 0x00,
	0x5180, 0x50,
	0x5184, 0x73,
	0x5185, 0x50,
	0x5186, 0x49,
	0x5187, 0xab,
	0x518b, 0x3f,
	0x518c, 0x26,
	0x3035, 0x21,
	0x3037, 0x12,
	0x3020, 0x09,
	0x3016, 0x18,
	0x3827, 0xec,
	0x4601, 0x16,
	0x4837, 0x28,
	0x3018, 0x04,
	0x5a24, 0x00,
	0x3a18, 0x00,
	0x3a19, 0xfe,
	0x3a1a, 0x06,
	0x3623, 0x03,
	0x3006, 0x12,
	0x3c00, 0x80,

	0x3c01, 0x34,
	0x3c04, 0x18,
	0x3c05, 0x70,
	0x3c06, 0x00,
	0x3c07, 0x20,
	0x3c08, 0x00,
	0x3c09, 0xc2,
	0x3c0a, 0x4e,
	0x3c0b, 0x20,
	0x380c, 0x03,
	0x380d, 0x14,
	0x380e, 0x01,
	0x380f, 0xfc,
	0x5091, 0x00,
	0x3810, 0x00,
	0x3811, 0x04,
	0x3812, 0x00,
	0x3813, 0x04,
	0x3808, 0x02,
	0x3809, 0x80,
	0x380a, 0x01,
	0x380b, 0xe0,
	0x3800, 0x00,
	0x3801, 0x04,
	0x3802, 0x00,
	0x3803, 0x00,
	0x3804, 0x02,
	0x3805, 0x8b,
	0x3806, 0x01,
	0x3807, 0xe7,
	0x4300, 0x30,
	0x501f, 0x01,
	0x4301, 0x80,
	0x4001, 0x02,
	0x4004, 0x04,
	0x4051, 0x00,
	0x4002, 0x45,
	0x3a0f, 0x58,
	0x3a10, 0x48,
	0x3a1b, 0x58,
	0x3a1e, 0x48,
	0x3632, 0xc9,
	0x0100, 0x01,
	0x5000, 0xcf,
	0x5001, 0xa7,
	0x5780, 0x5f,
	0x3c00, 0x04,
	0x4009, 0x0c,
	0x3a18, 0x01,
	0x3a19, 0x00,
	0x3a0c, 0xa4,
	0x5025, 0x01,
	0x3a05, 0x24,
	0x3a0f, 0x56,
	0x3a10, 0x4e,
	0x3a1b, 0x57,
	0x3a1e, 0x4d,
	0x5481, 0x0a,
	0x5482, 0x12,
	0x5483, 0x21,
	0x5484, 0x3d,
	0x5485, 0x4b,
	0x5486, 0x58,
	0x5487, 0x65,
	0x5488, 0x70,
	0x5489, 0x79,
	0x548a, 0x82,
	0x548b, 0x94,
	0x548c, 0xa3,
	0x548d, 0xbf,
	0x548e, 0xd6,
	0x548f, 0xeb,
	0x5490, 0x1c,
	0x5800, 0x00,
	0x5801, 0x00,
	0x5802, 0x01,
	0x5803, 0x48,
	0x5804, 0x00,
	0x5805, 0xf4,
	0x5806, 0x1c,
	0x5807, 0x21,
	0x5808, 0x2e,
	0x5809, 0x03,
	0x580a, 0x0c,
	0x580b, 0x30,
	0x5381, 0x22,
	0x5382, 0x3a,
	0x5383, 0x24,
	0x5384, 0x06,
	0x5385, 0x7a,
	0x5386, 0x80,
	0x5387, 0x6c,
	0x5388, 0x59,
	0x5389, 0x13,
	0x538b, 0x98,
	0x538a, 0x01,
	0x5308, 0x00,
	0x5300, 0x18,
	0x5301, 0x20,
	0x5302, 0x38,
	0x5303, 0x18,
	0x5304, 0x18,
	0x5305, 0x20,
	0x5306, 0x0a,
	0x5307, 0x20,
	0x5580, 0x02,
	0x5589, 0x18,
	0x558a, 0x00,
	0x558b, 0x80,
	0x5583, 0x3f,
	0x5584, 0x20,
	0x5180, 0x54,
	0x5182, 0x41,
	0x5183, 0x31,
	0x5184, 0x80,
	0x5185, 0x57,
	0x5186, 0xab,
	0x5187, 0x8d,
	0x5188, 0x11,
	0x5189, 0x1c,
	0x518a, 0x0e,
	0x518b, 0x43,
	0x518c, 0x2e,
	0x518d, 0xf0,
	0x518e, 0x04,
	0x519b, 0x02,
	0x518f, 0x52,
	0x5190, 0x00,
	0x5191, 0xff,
	0x5192, 0x00,
	0x5193, 0xff,
	0x5194, 0x00,
	0x0100, 0x01,
};
#endif