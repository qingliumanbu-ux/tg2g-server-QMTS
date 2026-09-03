/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-11-27 15:08:16
Description: 查询子母画面信息
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(form_helper)
int f_form_helper(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		CDbCommand cmd(conn);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "FORM_BASE_NAME");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "FORM_NAME");
		sqlstr = "SELECT FORM_BASE_NAME,FORM_NAME FROM TESFORMPARA WHERE FORM_BASE_NAME = @form_base_name";
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			cmd.Parameters.Set("form_base_name", bcls_rec->Tables[0].Rows[i]["FORM_BASE_NAME"]);
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteReader();
			int fechcount = 0;
			while (cmd.Read())
			{
				fechcount++;
				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[bcls_ret->Tables[0].Rows.get_Count() - 1]["FORM_BASE_NAME"] = cmd.GetString(1);
				bcls_ret->Tables[0].Rows[bcls_ret->Tables[0].Rows.get_Count() - 1]["FORM_NAME"] = cmd.GetString(2);
			}
			if (!fechcount)
			{
				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[bcls_ret->Tables[0].Rows.get_Count() - 1]["FORM_BASE_NAME"] = bcls_rec->Tables[0].Rows[i]["FORM_BASE_NAME"];
			}
		}
		Log::Trace("", "", "bcls_rec->Tables[0].Rows.count = {0}", bcls_rec->Tables[0].Rows.get_Count());
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
