/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-07-07 9:13:28
Description: 成分发送智慧质量
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t82305_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	EIClass tqmts29;
	try
	{
		CString epex_number = "T82305";
		CString deal_flag = "1";
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];

		//查询机组号
		CDbCommand cmd1(conn);
		CString sqlstr1 = "SELECT DEV_CODE,ST_NO FROM TQMTSB0 WHERE HEAT_NO = '" + heat_no + "'";
		cmd1.SetCommandText(sqlstr1);
		cmd1.ExecuteReader();
		CString dev_code = " ";
		CString st_no = " ";
		if (cmd1.Read())
		{
			dev_code = cmd1.GetString(1);
			st_no = cmd1.GetString(2);
		}
		cmd1.Close();

		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("HEAT_NO", 0, heat_no) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("ST_NO", 0, st_no) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("UNIT_CODE", 0, dev_code) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("START_TIME", 0, datetime) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CDbCommand cmd(conn);
		sqlstr = "SELECT * FROM TQMTS29 WHERE HEAT_NO = @heat_no AND ELM_ACT < 100 AND ELM_ACT > 0";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteQuery(tqmts29.Tables[0]);
		for (size_t i = 0; i < tqmts29.Tables[0].Rows.get_Count(); i++)
		{
			if (epex.SetValue("ELM_CODE", i, tqmts29.Tables[0].Rows[i]["ELM_CODE"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("ELM_NAME", i, tqmts29.Tables[0].Rows[i]["ELM_NAME"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("ELM_ACT", i, tqmts29.Tables[0].Rows[i]["ELM_ACT"].ToDecimal().Round(5).ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		if (epex.SendTele() < 0)
		{
			Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
			sprintf(s.msg, (const char*)epex.GetMsg());
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


