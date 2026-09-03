/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   yiling
Version:    1.0
Date:     2015-07-23
Description: 混杂元素电文接收
**************************************************/
/*<remark>=========================================================
/// <summary>
/// 混杂元素电文接收
/// <para>
/// 1.混杂元素电文接收
/// </para>
/// <para>数据库表：TQMTS09(混杂元素表)         </para>
/// <para>主调用函数：     </para>
/// </summary>
/// <returns>无</returns>
===========================================================</remark>*/

//框架公用头文件，勿删
#include "stdafx.h"
#include "epex.h"
#include "tqmts09.h"

BM2F_ENTERACE_TELE(cm_002002_rcv)
int f_cm_002002_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	// 程序用变量

	CString  v_flag = " ";
	int   doFlag = 0;
	CString lpsz_tc_no = " ";
	/* ***** 创建电文处理对象 ***** */
	EPEX epex(&s, conn);
	/* 使用的表结构变量 */
	CTQMTS09 tqmts09(conn);
	CString sqlstr;
	CDbCommand  cmd_inq(conn);



	try
	{
		tqmts09.Reset();
		v_flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToString().Trim();
		tqmts09.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "v_flag=[{0}]1：新增，2修改；3：删除", v_flag);
		Log::Info("", __FUNCTION__, "tqmts09.HZGL=[{0}]", tqmts09.HZGL);


		if (v_flag.Trim() == "1")
		{
			tqmts09.Insert();
		}
		else if (v_flag.Trim() == "2")
		{
			tqmts09.Delete("HZGL");
			tqmts09.Insert();
		}
		else if (v_flag.Trim() == "3")
		{
			tqmts09.Delete("HZGL");
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EITrace对象的sys_Trace.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
