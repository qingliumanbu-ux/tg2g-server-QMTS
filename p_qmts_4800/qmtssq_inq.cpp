/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024.8.13
Description: 上传文件查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtssq_inq)


int f_qmtssq_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString seq_no = bcls_rec->Tables[0].Rows[0]["TRUCK_SEQ_NO"].ToString().Trim();
		CString stoveid = bcls_rec->Tables[0].Rows[0]["C_STOVEID"].ToString().Trim();
		CString date_fo = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME"].ToString().Trim();
		CString date_to = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TO"].ToString().Trim();
		CString h_mat_no = bcls_rec->Tables[0].Rows[0]["HOT_MAT_NO"].ToString().Trim();


		Log::Trace("", "", "seq_no = {0}", seq_no);
		Log::Trace("", "", "stoveid = {0}", stoveid);
		Log::Trace("", "", "date_fo = {0}", date_fo);
		Log::Trace("", "", "date_to = {0}", date_to);
		Log::Trace("", "", "h_mat_no = {0}", h_mat_no);

		sqlstr = "SELECT * FROM  TQMTSSQ WHERE 1=1 ";
		if (seq_no != "")
		{
			sqlstr += " AND TRUCK_SEQ_NO  = @seq_no";
		}
		if (stoveid != "")
		{
			sqlstr += " AND  C_STOVEID = @stoveid";
		}
		if (date_fo != "")
		{
			sqlstr += " OR REC_CREATE_TIME >= @date_fo ";
		}
		if (date_to != "")
		{
			sqlstr += " OR REC_CREATE_TO <= @date_to ";
		}
		if (h_mat_no != "")
		{
			sqlstr += " AND HOT_MAT_NO = @h_mat_no ";
		}
	

		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("seq_no", seq_no);
		cmd.Parameters.Set("stoveid", stoveid);
		cmd.Parameters.Set("date_fo", date_fo);
		cmd.Parameters.Set("date_to", date_to);
		cmd.Parameters.Set("h_mat_no", h_mat_no);
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


