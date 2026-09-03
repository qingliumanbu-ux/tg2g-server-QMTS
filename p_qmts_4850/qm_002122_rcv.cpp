/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-7 15:08:16
Description: 工序制造标准_连铸（北）
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"


BM2F_ENTERACE_TELE(qm_002122_rcv)

int f_qm_002122_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmts08("TQMTS08");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		for (int i = 0; i < bcls_rec->Tables["tqmts08"].Rows.get_Count(); i++)
		{
			tqmts08.MergeFrom(bcls_rec->Tables["tqmts08"].Rows[i]);
			tqmts08["REC_CREATOR"] = s.userid;
			tqmts08["REC_CREATE_TIME"] = datetime;
			tqmts08.Delete("ST_NO,CROSS_SPEC,CC_SLAG");
			tqmts08.Insert();
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


