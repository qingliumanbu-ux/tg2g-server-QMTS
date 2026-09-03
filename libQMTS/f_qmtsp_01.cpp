/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-18
Description: 复制工艺卡的成分数据
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///复制工艺卡的成分数据
/// <para>
/// 1.复制工艺卡的成分数据。
/// 
/// </para>
/// <para>数据库表：TQMTS02(成分标准)						</para>
/// <para>主调用函数：该接口函数在各工序获取工艺卡时调用	</para>
/// <para>需调用函数：										</para>
/// </summary>
/// <param name="ST_NO">出钢记号							</param>
/// <param name="FACTORY_DIV">厂别区分						</param>
/// <param name="WHOLE_BACKLOG_CODE">工序代码				</param>
/// <returns>  </returns>
===========================================================</remark>*/


BM2_FUNCTION_EXPORT
 int f_qmtsp_01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	///程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts02("TQMTS02");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获得输入参数
		//*********************************   1. 获取/检查输入参数   *********************************//	    

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
