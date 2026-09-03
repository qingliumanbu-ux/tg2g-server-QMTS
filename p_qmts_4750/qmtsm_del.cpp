/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-17 15:08:16
Description: 制造标准删除，并且新增历史表
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm_del)
int f_qmtsm_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString table_name = bcls_rec->Tables[1].Rows[0]["TABLE_NAME"];
		CString whole_backlog_code = bcls_rec->Tables[1].Rows[0]["WHOLE_BACKLOG_CODE"];
		CModel tqmts("T" + table_name);
		CModel hqmts("H" + table_name);
		CModel tqmts02("TQMTS02");
		CModel hqmts02("HQMTS02");
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts["ST_NO"] = bcls_rec->Tables[0].Rows[i]["ST_NO"];
			tqmts["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[i]["FACTORY_DIV"];
			tqmts.Query("ST_NO,FACTORY_DIV");
			tqmts.Delete();
			hqmts.CopyFrom(tqmts);
			hqmts["DU_TIME"] = datetime;
			hqmts["DU_FLAG"] = "D";
			hqmts["DU_MAKER"] = s.userid;
			hqmts.Insert();
			sqlstr = "SELECT * FROM TQMTS02 WHERE ST_NO = @st_no AND WHOLE_BACKLOG_CODE = @whole_backlog_code AND FACTORY_DIV = @factory_div";
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("st_no", tqmts["ST_NO"]);
			cmd.Parameters.Set("whole_backlog_code", whole_backlog_code);
			cmd.Parameters.Set("factory_div", tqmts["FACTORY_DIV"]);
			cmd.ExecuteReader();
			while (cmd.Read())
			{
				cmd.Fetch(tqmts02);
				cmd.Fetch(hqmts02);
				hqmts02["DU_TIME"] = datetime;
				hqmts02["DU_FLAG"] = "D";
				hqmts02["DU_MAKER"] = s.userid;
				hqmts02.Insert();
				tqmts02.Delete("ST_NO,FACTORY_DIV,BASE_CODE,ELM_CODE,WHOLE_BACKLOG_CODE");
			}
			cmd.Close();
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


