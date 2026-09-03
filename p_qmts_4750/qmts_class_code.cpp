/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      ÅË³Â
Version:     1.0
Date:        2021-04-28 15:08:16
Description: ²éÑ¯Ð¡´úÂë
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts_class_code)
int f_qmts_class_code(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			CString code_class = bcls_rec->Tables[0].Rows[i]["CODE_CLASS"];
			sqlstr = "SELECT CODE_CLASS,CODE,CODE_DESC_1_CONTENT,CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT,CODE_DESC_4_CONTENT,CODE_DESC_5_CONTENT FROM TEP0002 WHERE CODE_CLASS =@code_class ";
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("code_class", code_class);
			bcls_ret->Tables.Add(code_class);
			cmd.ExecuteQuery(bcls_ret->Tables[code_class]);
		}
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
