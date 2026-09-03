/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-18 15:08:16
Description: 制造标准审核
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm_passcancel)
int f_qmtsm_passcancel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CString table_name = bcls_rec->Tables[1].Rows[0]["TABLE_NAME"];
		CModel tqmts("T" + table_name);
		CModel hqmts("H" + table_name);

		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tqmts.Query("ST_NO,FACTORY_DIV");
			hqmts.CopyFrom(tqmts);
			hqmts["DU_MAKER"] = s.userid;
			hqmts["DU_TIME"] = datetime;
			hqmts["DU_FLAG"] = "C";
			hqmts.Insert();
			Log::Trace("", "", "插入历史表H{0}", table_name);

			tqmts["VALID_FLAG"] = "0";
			tqmts["DU_MAKER"] = s.userid;
			tqmts["DU_TIME"] = datetime;
			tqmts["DU_FLAG"] = "C";
			tqmts.Update("VALID_FLAG,DU_MAKER,DU_TIME,DU_FLAG", "ST_NO,FACTORY_DIV");
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


