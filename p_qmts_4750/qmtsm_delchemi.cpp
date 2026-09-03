/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-17 15:08:16
Description: 制造标准删除，并且新增历史表
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm_delchemi)
int f_qmtsm_delchemi(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CString whole_backlog_code = bcls_rec->Tables[1].Rows[0]["WHOLE_BACKLOG_CODE"];
		CModel hqmts02("HQMTS02");
		CModel tqmts02("TQMTS02");
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts02.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tqmts02["WHOLE_BACKLOG_CODE"] = whole_backlog_code;
			tqmts02.Query("ST_NO,FACTORY_DIV,ELM_CODE,WHOLE_BACKLOG_CODE");
			tqmts02.Delete("ST_NO,FACTORY_DIV,ELM_CODE,WHOLE_BACKLOG_CODE");
			hqmts02.CopyFrom(tqmts02);
			hqmts02["DU_TIME"] = datetime;
			hqmts02["DU_FLAG"] = "D";
			hqmts02["DU_MAKER"] = s.userid;
			hqmts02.Insert();
		}
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


