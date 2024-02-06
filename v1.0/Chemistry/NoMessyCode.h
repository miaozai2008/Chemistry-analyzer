#pragma once
//处理中文乱码
#ifdef NDEBUG
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif
#endif