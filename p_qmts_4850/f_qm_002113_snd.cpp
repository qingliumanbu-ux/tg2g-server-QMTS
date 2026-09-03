/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-7 15:08:16
Description: 工序制造标准_预处理（北）
**************************************************/

#include "stdafx.h"
#include "epex.h"
#include "tqmts03.h"

BM2_FUNCTION_EXPORT

int f_qm_002113_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CTQMTS03 tqmts03(conn);
	CDbCommand cmd(conn);
	EPEX epex;
	CString lpsz_tc_no = "002113";
	CString st_no = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		if (epex.Initialize(lpsz_tc_no) < 0)
		{
			Log::Trace("", "", "f_qm_002113_rcv---initTele[{0}]", (const char*)epex.GetMsg());
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = "SELECT "
			"* "
			"FROM "
			"tqmts03 "
			"WHERE "
			"ST_NO = @st_no ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("st_no", st_no);
		cmd.ExecuteReader();
		int COUNT = 0;

		while (cmd.Read())
		{
			cmd.Fetch(tqmts03);
			if (epex.SetValue("tqmts03", COUNT, tqmts03) < 0)
			{
				sprintf(s.msg, "电文失败---tqmts03", epex.GetMsg());
				Log::Trace("", __FUNCTION__, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmts03", "st_no", COUNT, st_no) < 0)
			{
				Log::Trace("", "", "f_qm_002113_rcv---proc_div[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			COUNT = COUNT + 1;
		}
		cmd.Close();
		if (epex.SendTele() < 0)
		{
			sprintf(s.msg, "电文发送失败:%.460s", epex.GetMsg());
			Log::Trace("", __FUNCTION__, s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		epex.Uninitialize();
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


