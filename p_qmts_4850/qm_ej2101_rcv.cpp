/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-7 15:08:16
Description: 化学成分接收
**************************************************/
#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE_TELE(qm_ej2101_rcv)
int f_qmts_elm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//成分接收函数

int f_qm_ej2101_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		doFlag = f_qmts_elm_rcv(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			Log::Trace("", "", "f_qmts_elm_rcv() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
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


