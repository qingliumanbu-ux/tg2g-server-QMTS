/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-04-8 15:08:16
Description: 查询成分标准
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qmbsm8_ins)
int f_qmbs_save(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
int f_qmbsm8_ins(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		Log::Trace("", "", "line = {0}", __LINE__);
		f_epex_call_cgi_svc(conn, "TG0RM", "qmbsm8_inq", bcls_rec, bcls_ret, 120);
		struct ei_sys s_tmp;
		bcls_ret->GetSYS(&s_tmp);
		if (s_tmp.flag < 0)
		{
			strcpy(s.msg, s_tmp.msg);
			s.flag = s_tmp.flag;
			Log::Trace("", "", "调用失败. s.flag=[{0}] s.msg =[{1}] s.sysmsg=[{2}]", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = "delete from TQMTMS0";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteNonQuery();
		bcls_ret->Tables[0].set_TableName("QMTMS0_ADD");
		doFlag = f_qmbs_save(bcls_ret, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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
