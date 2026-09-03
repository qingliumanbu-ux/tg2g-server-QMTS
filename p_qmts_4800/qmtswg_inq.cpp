/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024.4.11
Description: 工艺卡违规查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtswg_inq)


int f_qmtswg_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		CString date_fo = bcls_rec->Tables[0].Rows[0]["DATE_TIME_FO"].ToString().Trim();
		CString date_to = bcls_rec->Tables[0].Rows[0]["DATE_TIME_TO"].ToString().Trim();
		CString desc = bcls_rec->Tables[0].Rows[0]["SAP_ZRDW_DESC"].ToString().Trim();
		

		Log::Trace("", "", "st_no = {0}", st_no);
		Log::Trace("", "", "heat_no = {0}", heat_no);
		Log::Trace("", "", "date_fo = {0}", date_fo);
		Log::Trace("", "", "date_to = {0}", date_to);
		Log::Trace("", "", "desc = {0}", desc);

		sqlstr = "SELECT * FROM  TQMTSWG WHERE 1=1 ";
		if (st_no != "")
		{
			sqlstr += " AND ST_NO  = @st_no";
		}
		if (heat_no != "")
		{
			sqlstr += " AND HEAT_NO  = @heat_no";
		}
		if (date_fo != "")
		{
			sqlstr += " OR RQ_TIME >= @date_fo ";
		}
		if (date_to != "")
		{
			sqlstr += " OR RQ_TIME <= @date_to ";
		}
		if (desc != "")
		{
			sqlstr += " AND SAP_ZRDW_DESC = @desc ";
		}
		sqlstr += "  ORDER BY KEY_SEQ ASC ";


		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("st_no", st_no);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.Parameters.Set("date_fo", date_fo);
		cmd.Parameters.Set("date_to", date_to);
		cmd.Parameters.Set("desc", desc);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);

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


