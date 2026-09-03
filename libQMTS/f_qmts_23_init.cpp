/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-05-16 9:54:28
Description: HEAT_NO和PONO绑定时，初始化TQMTS23
**************************************************/

#include "stdafx.h"


BM2_FUNCTION_EXPORT

int f_qmts_23_init(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	int ret;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts23("TQMTS23");//炉次质量信息表
	CDbCommand cmd(conn);
	try
	{
		tqmts23["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];

		/*tqmts23表初始化赋值*/
		if (tqmts23.Query("HEAT_NO"))
		{
			Log::Trace("", "", "炉次有质量信息表，程序结束");
			return 0;
		}
		tqmts23["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		tqmts23["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		tqmts23["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"].ToString();
		tqmts23["REC_CREATOR"] = s.userid;
		tqmts23["REC_CREATE_TIME"] = datetime;
		tqmts23["COMPANY_NAME"] = s.svc_name;
		//新增TQMTS23表
		tqmts23.TrimOrBlank();
		tqmts23.Insert();
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


