/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 校验工艺卡是否已更新
**************************************************/

///框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 校验工艺卡是否已更新
/// <para>
/// 1.校验工艺卡是否已更新。
/// 
/// </para>
/// <para>数据库表：TQMTS0X（工艺卡）				        </para>
/// <para>主调用函数：该接口函数在制造标准查询时调用
/// <para>需调用函数： 
/// </summary>
/// <param name="st_no">出钢记号                 </param>
/// <param name="factory_div">厂别区分           </param> 
/// <param name="catch_time">工艺卡获取时间      </param> 
/// <returns> "FLAG" 1-工艺卡已更新 0-工艺卡未更新 </returns>
===========================================================</remark>*/

BM2_FUNCTION_EXPORT
 int f_qmtsp_00(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//变量声明
	CString sqlstr(""); 
	int doFlag = 0; 
	int i = 0;
	int flag;

	CString q_st_no = " ";
	CString q_factory_div = " ";	
	CString q_catch_time = " ";
	CDecimal v_count = 0;

	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{	

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
