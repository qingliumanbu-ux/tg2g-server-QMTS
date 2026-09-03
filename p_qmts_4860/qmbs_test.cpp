/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2021-06-27 15:08:16
Description: 处理一些急活
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qmbs_test)
int f_t82309_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送低倍实绩电文
int f_qmbs_test(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		Log::Trace("", "", "line ={0}", __LINE__);
		sqlstr = " SELECT HEAT_NO,MAT_NO  FROM  TQMTS27"
			"			WHERE 	HEAT_NO IN('B3402816','B5403873','A0401573','A0401811',"
			"'B1403652', 'B2403489', 'B1403564', 'B2403437', 'B2403427', 'B1403335', 'B2403437', 'B2403488', 'B4403965'"
			" ) ";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
		EIClass heat_class;
		heat_class.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		heat_class.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
		heat_class.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		
		heat_class.Tables[0].Rows.Add();
		for (size_t i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			heat_class.Tables[0].Rows[0]["HEAT_NO"] = bcls_ret->Tables[0].Rows[i]["HEAT_NO"];
			heat_class.Tables[0].Rows[0]["DEAL_FLAG"] = "I";
			heat_class.Tables[0].Rows[0]["MAT_NO"] = bcls_ret->Tables[0].Rows[i]["MAT_NO"];
			//发送智慧质量电文
			doFlag = f_t82309_snd(&heat_class, bcls_rec, conn);
			if (doFlag != 0)
			{
				Log::Trace("", "", "f_t82309_snd() msg = [{0}]", s.msg);
				s.flag = -1;
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
